#pragma once
#include <sstream>
#include <memory>
#include <vector>
#include <cstdint>
#include "NodeValue/ASTNodePtr.h"

#include "BytecodeSerializationContext.h"
#include "BytecodeChunk.h"
#include "Generator/Value/BytecodeScriptCache.h"

namespace ska {
	namespace bytecode {
		struct SerializationContext {
			SerializationContext(const ScriptCache& cache, std::size_t generated, std::ostream& output) :
				m_id(generated),
				m_output(output),
				m_cache(cache) {
			}

			auto begin() const { return m_cache[m_id].begin(); }
			auto end() const { return m_cache[m_id].end(); }

			const auto& exports() const { return m_cache[m_id].exportedSymbols(); }

			bool next() {
				pushNatives();
				m_id++;
			 return m_id < m_cache.size();
			}

			const std::string& currentScriptName() const { return m_cache[m_id].name(); }
			std::size_t currentScriptId() const { return m_id; }

			std::ostream& operator<<(std::size_t value) {
				m_output.write(reinterpret_cast<const char*>(&value), sizeof(uint32_t));
				return m_output;
			}

			std::ostream& operator<<(std::string value) {
				const Chunk refIndex = m_natives.size();
				m_output.write(reinterpret_cast<const char*>(&refIndex), sizeof(Chunk));
				m_natives.push_back(std::move(value));
				return m_output;
			}

			std::ostream& operator<<(const Instruction& value) {
				uint16_t cmd = static_cast<uint16_t>(value.command());
				uint8_t numberOfValidOperands = !value.dest().empty() + !value.left().empty() + !value.right().empty();
				m_output.write(reinterpret_cast<const char*>(&cmd), sizeof(uint16_t));
				m_output.write(reinterpret_cast<const char*>(&numberOfValidOperands), sizeof(uint8_t));

				if (numberOfValidOperands-- > 0) *this << value.dest(); else return m_output;
				if (numberOfValidOperands-- > 0) *this << value.left(); else return m_output;
				if (numberOfValidOperands-- > 0) *this << value.right();
				return m_output;
			}

			std::ostream& operator<<(const Operand& value) {
				uint8_t type = static_cast<uint8_t>(OperandType::EMPTY);
				Chunk script { 0 };
				Chunk variable { 0 };

				if (value.empty()) {
					m_output.write(reinterpret_cast<const char*>(&type), sizeof(uint8_t));
					char empty[sizeof(Chunk) * 2] = "";
					m_output.write(empty, sizeof(empty));
					return m_output;
				}

				const auto& content = value.content();
				if (std::holds_alternative<StringShared>(content)) {
					type = static_cast<uint8_t>(OperandType::PURE);
					script = 0;
					m_output.write(reinterpret_cast<const char*>(&type), sizeof(uint8_t));
					m_output.write(reinterpret_cast<const char*>(&script), sizeof(Chunk));
					*this << *std::get<StringShared>(content);
				} else {
					type = static_cast<uint8_t>(value.type());

					std::visit([&](const auto& operand) {
						using TypeT = std::decay_t<decltype(operand)>;
						if constexpr (std::is_same_v<ScriptVariableRef, TypeT>) {
							script = operand.script;
							variable = operand.variable;
						} else if constexpr (!std::is_same_v<StringShared, TypeT>) {
							if constexpr (std::is_same_v<long, TypeT>) {
								script = static_cast<Chunk>(1);
							} else if constexpr (std::is_same_v<double, TypeT>) {
								script = static_cast<Chunk>(2);
							} else {
								script = static_cast<Chunk>(3);
							}
							variable = static_cast<Chunk>(operand);
						}
					}, content);

					m_output.write(reinterpret_cast<const char*>(&type), sizeof(uint8_t));
					m_output.write(reinterpret_cast<const char*>(&script), sizeof(Chunk));
					m_output.write(reinterpret_cast<const char*>(&variable), sizeof(Chunk));
				}
				return m_output;
			}

		private:
			void pushNatives() {
				for (auto& native : m_natives) {
					if (! native.empty()) {
						const std::size_t size = native.size();
						m_output.write(reinterpret_cast<const char*>(&size), sizeof(Chunk));
						if (size > 0) {
							m_output.write(native.c_str(), sizeof(char) * size);
						}
					}
				}
				char empty[sizeof(Chunk)] = "";
				m_output.write(empty, sizeof(empty));
				m_natives.clear();
			}

			const ScriptCache& m_cache;
			std::size_t m_id;
			std::vector<std::string> m_natives;
			std::ostream& m_output;
		};

		struct DeserializationContext {
			DeserializationContext(ScriptCache& cache, std::istream& input) :
				m_id(0),
				m_input(input),
				m_cache(cache) {
			}

			void declare(std::string scriptName, std::vector<Instruction> instructions) {
				auto output = InstructionOutput {};
				for(auto& ins : instructions) {
					output.push(std::move(ins));
				}
				auto fakeAST = ska::ScriptAST{ m_cache.astCache, scriptName, {Token {}} };
				m_cache.emplace(scriptName, ScriptGeneration{ ScriptGenerationHelper{m_cache, fakeAST }, std::move(output)});
			}
			void exports(std::vector<Operand> exports) { m_cache[m_id].setExportedSymbols(std::move(exports)); }

			void operator>>(std::size_t& value) {
				value = 0;
				if(m_input.eof()) {
					return;
				}
				m_input.read(reinterpret_cast<char*>(&value), sizeof(uint32_t));
			}

			void operator>>(Chunk& value) {
				if(m_input.eof()) {
					value = Chunk{};
					return;
				}
				m_input.read(reinterpret_cast<char*>(&value), sizeof(Chunk));
			}

			void operator>>(Instruction& value) {
				auto operands = std::vector<Operand>{};
				auto numberOfValidOperands = std::uint8_t{ 0 };
				auto command = uint16_t { 0 };

				if(m_input.eof()) {
					value = Instruction{};
					return;
				}

				m_input.read(reinterpret_cast<char*>(&command), sizeof(uint16_t));
				m_input.read(reinterpret_cast<char*>(&numberOfValidOperands), sizeof(uint8_t));
				for(uint8_t i = 0; i < numberOfValidOperands; i++) {
					auto tmp = Operand {};
					*this >> tmp;
					operands.push_back(std::move(tmp));
				}
				value = Instruction{ static_cast<Command>(command), std::move(operands) };
			}

			void operator>>(Operand& value) {
				auto type = uint8_t { 0 };
				auto script = Chunk{ 0 };
				auto variable = Chunk{ 0 };

				if(m_input.eof()) {
					value = Operand{};
					return;
				}

				m_input.read(reinterpret_cast<char*>(&type), sizeof(uint8_t));
				m_input.read(reinterpret_cast<char*>(&script), sizeof(Chunk));
				m_input.read(reinterpret_cast<char*>(&variable), sizeof(Chunk));

				auto operandType = static_cast<OperandType>(type);
				auto content = OperandVariant {};
				switch (operandType) {
					case OperandType::PURE:
						switch (static_cast<std::size_t>(script)) {
						case 0:
							//native ref
							content = ScriptVariableRef { static_cast<std::size_t>(variable), static_cast<std::size_t>(0) };
							operandType = OperandType::MAGIC;
							break;
						case 1:
							content = static_cast<long>(variable);
							break;
						case 2:
							content = static_cast<double>(variable);
							break;
						case 3:
							content = static_cast<bool>(variable);
							break;
						}
					break;

					case OperandType::BIND:
					case OperandType::REG:
					case OperandType::VAR:
						content = ScriptVariableRef{ static_cast<std::size_t>(variable), static_cast<std::size_t>(script) };
						break;

					case OperandType::EMPTY:
						value = Operand {};
						return;

					default:
						throw std::runtime_error("cannot handle operand type deserialization " + std::to_string(type));
						break;
				}
				value = Operand{ std::move(content), operandType };
			}

			bool canRead() const { return !m_input.eof(); }

			void operator>>(std::vector<std::string>& natives) {
				if(m_input.eof()){
					return;
				}

				do {
					auto native = readString();
					natives.push_back(std::move(native));
				} while (!natives.back().empty());
			}

		private:
			std::string readString() {
				auto value = std::string {};
				auto size = Chunk { 0 };
				m_input.read(reinterpret_cast<char*>(&size), sizeof(Chunk));
				if (size > 0) {
					value.resize(size);
					m_input.read(reinterpret_cast<char*>(&value[0]), sizeof(char)* size);
				}
				return value;
			}

			ScriptCache& m_cache;
			std::size_t m_id;
			std::istream& m_input;
		};

	}
}

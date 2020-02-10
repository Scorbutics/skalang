#pragma once
#include <sstream>
#include <memory>
#include <vector>
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

			bool next() { m_id++; return m_id < m_cache.size(); }

			const std::string& currentScriptName() const { return m_cache[m_id].name(); }
			std::size_t currentScriptId() const { return m_id; }

			std::ostream& operator<<(const Chunk& value) {
				m_output.write(reinterpret_cast<const char*>(&value), sizeof(Chunk));
				return m_output;
			}

			std::ostream& operator<<(const std::string& value) {
				const std::size_t size = value.size();
				m_output.write(reinterpret_cast<const char*>(&size), sizeof(std::size_t));
				if (size > 0) {
					m_output.write(value.c_str(), sizeof(char) * size);
				}
				return m_output;
			}

		private:
			const ScriptCache& m_cache;
			std::size_t m_id;
			std::ostream& m_output;
		};

		struct DeserializationContext {
			DeserializationContext(ScriptCache& cache, std::size_t generated, std::istream& input) :
				m_id(generated),
				m_input(input),
				m_cache(cache) {
			}

			void exports(std::vector<Operand> exports) { m_cache[m_id].setExportedSymbols(std::move(exports)); }

			void operator>>(std::size_t& value) {
				m_input.read(reinterpret_cast<char*>(&value), sizeof(std::size_t));
			}

			void operator>>(Instruction& value) {
				auto dest = Operand {};
				auto left = Operand {};
				auto right = Operand {};
				auto command = Command {};
				m_input.read(reinterpret_cast<char*>(&command), sizeof(Command));
				*this >> dest;
				*this >> left;
				*this >> right;
				m_input.read(reinterpret_cast<char*>(&left), sizeof(Operand));
				m_input.read(reinterpret_cast<char*>(&right), sizeof(Operand));
				value = Instruction{ command, std::move(dest), std::move(left), std::move(right) };
			}

			void operator>>(std::string& value) {
				auto size = std::size_t {};
				m_input.read(reinterpret_cast<char*>(&size), sizeof(std::size_t));
				if (size > 0) {
					value.resize(size);
					m_input.read(reinterpret_cast<char*>(&value[0]), sizeof(char)* size);
				} else {
					value.clear();
				}
			}

			void operator>>(Operand& value) {
				auto type = Chunk {};
				auto script = Chunk{};
				auto variable = Chunk{};

				m_input.read(reinterpret_cast<char*>(&type), sizeof(Chunk));
				m_input.read(reinterpret_cast<char*>(&script), sizeof(Chunk));
				m_input.read(reinterpret_cast<char*>(&variable), sizeof(Chunk));

				auto content = OperandVariant {};
				auto operandType = static_cast<OperandType>(type);
				switch (operandType) {
					case OperandType::PURE:
						switch (static_cast<std::size_t>(script)) {
						case 0:
							//native ref
							content = ScriptVariableRef { static_cast<std::size_t>(variable), static_cast<std::size_t>(0) };
							operandType = OperandType::BIND;
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

					case OperandType::REG:
					case OperandType::VAR:
						content = ScriptVariableRef{ static_cast<std::size_t>(variable), static_cast<std::size_t>(script) };
						break;

					case OperandType::EMPTY:
						throw std::runtime_error("cannot handle EMPTY operand type deserialization");
						break;

					default:
						throw std::runtime_error("cannot handle operand type deserialization");
						break;
				}
				value = Operand{ std::move(content), operandType };
			}

			bool canRead() const { return !m_input.eof(); }

		private:
			ScriptCache& m_cache;
			std::size_t m_id;
			std::istream& m_input;
		};

	}
}

#pragma once
#include <memory>
#include <unordered_map>
#include "Value/PlainMemoryTable.h"
#include "Value/InstructionMemoryTable.h"
#include "Generator/Value/BytecodeGenerationOutput.h"

namespace ska {
	namespace bytecode {
		class ExecutionOutput;
		class ScriptExecution {
		public:
			ScriptExecution(ExecutionOutput& execution, const GenerationOutput& instructions, std::size_t scriptIndex) :
				instructions(instructions),
				execution(execution),
				scriptIndex(scriptIndex) {
			}

			ScriptExecution(const ScriptExecution&) = delete;
			ScriptExecution& operator=(const ScriptExecution&) = delete;

			const Instruction& currentInstruction() const { assert(executionPointer < instructions.generated()[scriptIndex].size()); return instructions.generated()[scriptIndex][executionPointer]; }

			bool incInstruction() {
				return ++executionPointer < instructions.generated()[scriptIndex].size();
			}

			NodeValue getCell(const Value& v) const;

			void jumpAbsolute(std::size_t value);
			void jumpRelative(long value);
			void jumpReturn();

			std::size_t getRelativeInstruction(long relativeValue) const {
				return executionPointer + relativeValue;
			}

			auto size() const { return instructions.generated()[scriptIndex].size(); }

			template <class T>
			T get(const Value& v) const {
				auto* memory = selectMemory(v);
				if(memory == nullptr) {
					if constexpr (NodeValue::is_container_of_values<T>()) {
						throw std::runtime_error("invalid get cell value usage by querying a variable container without a valid value provided");
					} else if constexpr (Value::is_member_of_values<T>()) {
						return v.as<T>();
					} else {
						return T{};
					}
				}
				return (*memory)[std::get<std::size_t>(v.as<VariableRef>())].nodeval<T>();
			}

			template <class T>
			void set(const Value& dest, T&& src) {
				auto* memory = selectMemory(dest);
				if(memory == nullptr) { throw std::runtime_error("invalid bytecode destination cell"); }
				push(*memory, dest, std::forward<T>(src));
			}

		private:
			PlainMemoryTable* selectMemory(const Value& dest);
			const PlainMemoryTable* selectMemory(const Value& dest) const;

			template<class IN, class OUT>
			static OUT SelectMemoryHelper(IN& self, const Value& dest) {
				switch (dest.type()) {
				case ValueType::PURE:
				default:
					return nullptr;
				case ValueType::REG:
					return &self.registers;
				case ValueType::VAR:
					return &self.execution.variables;
				case ValueType::EMPTY:
					throw std::runtime_error("cannot select empty variable relative memory");
				}
			}

			template <class T>
			void push(PlainMemoryTable& memory, const Value& dest, T&& src) {
				auto index = std::get<std::size_t>(dest.as<VariableRef>());
				if(index >= memory.size()) {
					if(index == memory.size()) {
						memory.push_back(std::forward<T>(src));
						return;
					} else {
						memory.resize(index + 1);
					}
				}
				memory[index] = std::forward<T>(src);
			}

			const GenerationOutput& instructions;
			ExecutionOutput& execution;
			std::size_t scriptIndex = 0;
			std::size_t executionPointer = 0;

			PlainMemoryTable registers;
		};
	}
}

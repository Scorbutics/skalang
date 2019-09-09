#pragma once
#include "Value/PlainMemoryTable.h"
#include "Value/InstructionMemoryTable.h"
#include "Generator/Value/BytecodeGenerationOutput.h"

namespace ska {
	namespace bytecode {
		struct ScriptExecution {
			ScriptExecution(GenerationOutput& instructions) :
				instructions(instructions) {
			}

			ScriptExecution(const ScriptExecution&) = delete;
			ScriptExecution& operator=(const ScriptExecution&) = delete;

			const Instruction& currentInstruction() const { assert(executionPointer < instructions.size()); return instructions[executionPointer]; }
			Instruction& currentInstruction() { assert(executionPointer < instructions.size()); return instructions[executionPointer]; }

			bool incInstruction() {
				return ++executionPointer < instructions.size();
			}

			NodeValue getCell(const Value& v);

			void pop(NodeValue& dest) {
				dest = stack.back();
				stack.pop_back();
			}

			void pop(NodeValueArrayRaw& dest, long count) {
				for(auto i = 0; i < count && stack.size() > 0; i++) {
					dest.push_front(stack.back());
					stack.pop_back();
				}
			}

			void jumpAbsolute(std::size_t value);
			void jumpRelative(long value);
			void jumpReturn();

			std::size_t getRelativeInstruction(long relativeValue) const {
				return executionPointer + relativeValue;
			}

			auto size() const { return instructions.size(); }

			template <class ... Items>
			void push(Items&& ... items) {
				(pushIfNotEmpty(std::forward<decltype(items)>(items)), ...);
			}

			template <class T>
			T get(const Value& v) {
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
			void pushIfNotEmpty(NodeValue value) {
				if(!value.empty()) {
					stack.push_back(std::move(value));
				}
			}

			PlainMemoryTable* selectMemory(const Value& dest) {
				switch(dest.type()) {
					case ValueType::PURE:
					default:
						return nullptr;
					case ValueType::REG:
						return &registers;
					case ValueType::VAR:
						return &variables;
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

			GenerationOutput& instructions;
			std::size_t executionPointer = 0;

			PlainMemoryTable variables;
			PlainMemoryTable registers;
			PlainMemoryTable stack;
			PlainMemoryTable callstack;
		};
	}
}

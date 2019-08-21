#pragma once

#include <cassert>
#include "Generator/Value/BytecodeGenerationOutput.h"
#include "Generator/Value/BytecodeValue.h"
#include "Value/PlainMemoryTable.h"
#include "Value/InstructionMemoryTable.h"
#include "Value/TokenVariant.h"

namespace ska {
	namespace bytecode {

		class ExecutionContext {
		public:
			ExecutionContext(GenerationOutput& instructions) :
				instructions(instructions) {
			}

			ExecutionContext(const ExecutionContext&) = delete;
			ExecutionContext& operator=(const ExecutionContext&) = delete;

			bool empty() const { return instructions.size() == 0; }

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
					dest.push_back(stack.back());
					stack.pop_back();
				}
			}

			long markLabel() {
				labels.push_back(executionPointer);
				return static_cast<long>(executionPointer);
			}

			void jumpAbsolute(std::size_t value) {
				executionPointer = value;
			}

			void jumpRelative(long value) {
				executionPointer += value;
			}

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
					} else {
						return v.as<T>();
					}
				}
				return (*memory)[v.as<VariableRef>()].nodeval<T>();
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
				switch(dest.type) {
					case ValueType::PURE:
					default:
						return nullptr;
					case ValueType::REG:
						return &registers;
					case ValueType::LBL:
						return &labels;
					break;
					case ValueType::VAR:
						return &variables;
				}
			}

			template <class T>
			void push(PlainMemoryTable& memory, const Value& dest, T&& src) {
				auto index = dest.as<std::size_t>();
				if(index >= memory.size()) {
					if(index == memory.size()) {
						memory.push_back(std::forward<T>(src));
						return;
					} else {
						memory.resize(index);
					}
				}
				memory[index] = std::forward<T>(src);
			}

			GenerationOutput& instructions;
			std::size_t executionPointer = 0;

			PlainMemoryTable variables;
			PlainMemoryTable registers;
			PlainMemoryTable stack;
			PlainMemoryTable labels;
		};
	}
}

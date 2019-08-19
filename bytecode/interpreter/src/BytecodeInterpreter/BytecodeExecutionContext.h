#pragma once

#include <cassert>
#include "Generator/Value/BytecodeGenerationOutput.h"
#include "Generator/Value/BytecodeValue.h"
#include "Value/PlainMemoryTable.h"
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

			const Instruction& currentInstruction() const { assert(executionPointer >= 0 && executionPointer < instructions.size()); return instructions[executionPointer]; }
			bool incInstruction() {
				return ++executionPointer < instructions.size();
			}

			auto& getVariant(const Value& v) {
				auto* memory = selectMemory(v);
				if(memory == nullptr) { throw std::runtime_error("invalid bytecode destination cell"); }
				return (*memory)[v.as<std::size_t>()];
			}

			template <class T>
			T get(const Value& v) {
				auto* memory = selectMemory(v);
				if(memory == nullptr) {
					return v.as<T>();
				}
				return (*memory)[v.as<std::size_t>()].nodeval<T>();
			}

			template <class T>
			void set(const Value& dest, T&& src) {
				auto* memory = selectMemory(dest);
				if(memory == nullptr) { throw std::runtime_error("invalid bytecode destination cell"); }
				push(*memory, dest, std::forward<T>(src));
			}

		private:
			PlainMemoryTable* selectMemory(const Value& dest) {
				switch(dest.type) {
					case ValueType::PURE:
					default:
						return nullptr;
					case ValueType::REG:
						return &registers;
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
		};
	}
}

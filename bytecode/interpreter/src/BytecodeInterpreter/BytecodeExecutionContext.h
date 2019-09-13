#pragma once

#include <cassert>
#include "Generator/Value/BytecodeValue.h"
#include "Value/TokenVariant.h"
#include "BytecodeInterpreter/Value/BytecodeExecutionOutput.h"
#include "BytecodeInterpreter/Value/ScriptExecutionOutput.h"

namespace ska {
	namespace bytecode {

		class GenerationOutput;
		class Generator;
		class ExecutionContext {
		public:
			ExecutionContext(ExecutionOutput& container, std::size_t scriptIndex, GenerationOutput& instructions);

			ExecutionContext(ExecutionContext& old, std::size_t scriptIndex) :
				ExecutionContext(old.m_container, scriptIndex, old.m_bytecode) { }

			ExecutionContext(const ExecutionContext&) = delete;
			ExecutionContext& operator=(const ExecutionContext&) = delete;

			bool empty() const { return m_current->size() == 0; }

			const Instruction& currentInstruction() const { return m_current->currentInstruction(); }

			bool incInstruction() { return m_current->incInstruction(); }

			NodeValue getCell(const Value& v) const { return m_current->getCell(v); }

			void pop(NodeValue& dest) { m_container.pop(dest); }

			void pop(NodeValueArrayRaw& dest, long count) { m_container.pop(dest, count); }

			void jumpAbsolute(std::size_t value) { m_current->jumpAbsolute(value); }
			void jumpRelative(long value) { m_current->jumpRelative(value); }
			void jumpReturn() { m_current->jumpReturn(); }

			std::size_t getRelativeInstruction(long relativeValue) const {
				return m_current->getRelativeInstruction(relativeValue);
			}

			template <class ... Items>
			void push(Items&& ... items) {
				m_container.push(std::forward<Items>(items)...);
			}

			template <class T>
			T get(const Value& v) {
				return m_current->get<T>(v);
			}

			template <class T>
			void set(const Value& dest, T&& src) {
				m_current->set(dest, std::forward<T>(src));
			}
		
			ScriptExecutionOutput generateExportedVariables(std::size_t scriptIndex) const;

			const ScriptGenerationOutput& generateIfNeeded(Generator& generator, std::size_t scriptIndex);

		private:
			ExecutionOutput& m_container;
			GenerationOutput& m_bytecode;
			ScriptExecution* m_current = nullptr;
		};
	}
}

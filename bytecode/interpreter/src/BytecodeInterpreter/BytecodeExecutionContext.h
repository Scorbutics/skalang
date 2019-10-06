#pragma once

#include <cassert>
#include "Generator/Value/BytecodeValue.h"
#include "Runtime/Value/TokenVariant.h"
#include "BytecodeInterpreter/Value/BytecodeExecutionOutput.h"
#include "BytecodeInterpreter/Value/ScriptExecutionOutput.h"
#include "BytecodeScriptExecution.h"

namespace ska {
	namespace bytecode {

		class GenerationOutput;
		class Generator;
		class ExecutionContext {
		public:
			ExecutionContext(ExecutionOutput& container, std::size_t scriptIndex, GenerationOutput& instructions);

			ExecutionContext(ExecutionContext& old, std::size_t scriptIndex) :
				ExecutionContext(old.m_container, scriptIndex, old.m_bytecode) { }

			ExecutionContext(ExecutionContext&&) noexcept = default;
			ExecutionContext& operator=(ExecutionContext&&) = default;
			ExecutionContext(const ExecutionContext&) = delete;
			ExecutionContext& operator=(const ExecutionContext&) = delete;

			bool idle() const { return m_current->idle(); }

			const Instruction& currentInstruction() const { return m_current->currentInstruction(); }

			bool incInstruction() { return m_current->incInstruction(); }

			NodeValue getCell(const Value& v) const { return scriptFromValue(v).getCell(v); }

			void pop(NodeValue& dest) { m_container.pop(dest); }

			void pop(NodeValueArrayRaw& dest, long count) { m_container.pop(dest, count); }

			void jumpAbsolute(ScriptVariableRef value);
			void jumpRelative(long value) { m_current->jumpRelative(value); }
			void jumpReturn();

			ScriptVariableRef getRelativeInstruction(long relativeValue) const {
				return m_current->getRelativeInstruction(relativeValue);
			}

			template <class ... Items>
			void push(Items&& ... items) {
				m_container.push(std::forward<Items>(items)...);
			}

			template <class T>
			T get(const Value& v) {
				return scriptFromValue(v).get<T>(v);
			}

			ScriptExecution& scriptFromValue(const Value& v);
			const ScriptExecution& scriptFromValue(const Value& v) const;

			template <class T>
			void set(const Value& dest, T&& src) {
				scriptFromValue(dest).set(dest, std::forward<T>(src));
			}

			ScriptExecutionOutput generateExportedVariables(std::size_t scriptIndex);

			const ScriptGenerationOutput& generateIfNeeded(Generator& generator, std::size_t scriptIndex);

		private:
			ScriptVariableRef getReturn();
			ExecutionContext getContext(ScriptVariableRef value);
			ExecutionOutput& m_container;
			GenerationOutput& m_bytecode;
			ScriptExecution* m_current = nullptr;
		};
	}
}

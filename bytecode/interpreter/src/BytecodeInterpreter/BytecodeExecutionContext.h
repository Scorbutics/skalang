#pragma once

#include <cassert>
#include "Generator/Value/BytecodeOperand.h"
#include "Runtime/Value/TokenVariant.h"
#include "BytecodeInterpreter/Value/BytecodeExecutor.h"
#include "BytecodeInterpreter/Value/ScriptExecutionOutput.h"
#include "BytecodeScriptExecution.h"
#include "Generator/Value/BytecodeGenerationOutput.h"

namespace ska {
	namespace bytecode {

		class Generator;
		class ExecutionContext {
		public:
			ExecutionContext(Executor& container, std::size_t scriptIndex, GenerationOutput& instructions);

			ExecutionContext(ExecutionContext& old, std::size_t scriptIndex) :
				ExecutionContext(old.m_out, scriptIndex, old.m_in) { }

			ExecutionContext(ExecutionContext&&) noexcept = default;
			ExecutionContext(const ExecutionContext&) = delete;
			ExecutionContext& operator=(const ExecutionContext&) = delete;

			bool idle() const { return m_current->idle(); }

			const Instruction& currentInstruction() const { return m_current->currentInstruction(); }

			bool incInstruction() { return m_current->incInstruction(); }

			NodeValue getCell(const Operand& v) const { return scriptFromOperand(v).getCell(v); }

			void pop(NodeValue& dest) { m_out.pop(dest); }

			void pop(NodeValueArrayRaw& dest, long count) { m_out.pop(dest, count); }

			void jumpAbsolute(ScriptVariableRef value);
			void jumpRelative(long value) { m_current->jumpRelative(value); }
			void jumpReturn();

			ScriptVariableRef getRelativeInstruction(long relativeValue) const {
				return m_current->getRelativeInstruction(relativeValue);
			}

			template <class ... Items>
			void push(Items&& ... items) {
				m_out.push(std::forward<Items>(items)...);
			}

			template <class T>
			T get(const Operand& v) {
				return scriptFromOperand(v).get<T>(v);
			}

			ScriptExecution& scriptFromOperand(const Operand& v);
			const ScriptExecution& scriptFromOperand(const Operand& v) const;

			template <class T>
			void set(const Operand& dest, T&& src) {
				scriptFromOperand(dest).set(dest, std::forward<T>(src));
			}

			ScriptExecutionOutput generateExportedVariables(std::size_t scriptIndex);

			const ScriptGeneration& generateIfNeeded(Generator& generator, std::size_t scriptIndex);

		private:
			ScriptVariableRef getReturn();
			ExecutionContext getContext(ScriptVariableRef value);

			Executor& m_out;
			GenerationOutput& m_in;
			ScriptExecution* m_current = nullptr;
		};
	}
}

#pragma once

#include <cassert>
#include "Generator/Value/BytecodeOperand.h"
#include "Runtime/Value/TokenVariant.h"
#include "BytecodeInterpreter/Value/BytecodeExecutor.h"
#include "BytecodeInterpreter/Value/ScriptExecutionOutput.h"
#include "BytecodeScriptExecution.h"
#include "Generator/Value/BytecodeGenerationOutput.h"

namespace ska {
	class StatementParser;
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

			bool idle() const { return m_current == nullptr ? true : m_current->idle(); }

			const Instruction& currentInstruction() const {
				checkCurrentExecutionOrThrow();
				return m_current->currentInstruction();
			}

			std::size_t currentScriptId() const {
				checkCurrentExecutionOrThrow();
				return m_current->id();
			}

			const std::string& currentScriptName() const {
				checkCurrentExecutionOrThrow();
				return m_current->name();
			}

			bool incInstruction() { return m_current == nullptr ? false : m_current->incInstruction(); }

			NodeValue getCell(const Operand& v) const;

			void pop(NodeValue& dest) { m_out.pop(dest); }
			void pop(NodeValueArrayRaw& dest, long count) { m_out.pop(dest, count); }

			void pushInEnv(const ScriptVariableRef& dest, NodeValue value) { m_out.pushInEnv(dest, std::move(value)); }

			void jumpAbsolute(NodeValue& value);
			void jumpRelative(long value) { checkCurrentExecutionOrThrow(); m_current->jumpRelative(value); }
			NodeValue jumpReturn();

			ScriptVariableRef getRelativeInstruction(long relativeValue) const {
				checkCurrentExecutionOrThrow();
				return m_current->getRelativeInstruction(relativeValue);
			}

			template <class ... Items>
			void push(Items&& ... items) {
				m_out.push(std::forward<Items>(items)...);
			}

			template <class T>
			const T& get(const Operand& variable) {
				const auto* value = variable.type() == OperandType::VAR || variable.type() == OperandType::REG ? useFromCurrentEnv(variable.as<ScriptVariableRef>()) : nullptr;
				return value == nullptr || value->empty() ? scriptFromOperand(variable).get<T>(variable) : value->nodeval<T>();
				//return scriptFromOperand(variable).get<T>(variable);
			}

			ScriptExecution& scriptFromOperand(const Operand& v);
			const ScriptExecution& scriptFromOperand(const Operand& v) const;

			template <class T>
			void set(const Operand& dest, T&& src) {
				if (m_out.closureEnvironment.empty()) {
					scriptFromOperand(dest).set(dest, std::forward<T>(src));
					return;
				}

				// TODO store env in script and access it with scriptFromOperand?
				auto* currentEnvironment = m_out.closureEnvironment.back();
				if (currentEnvironment != nullptr) {
					currentEnvironment->push(dest.as<ScriptVariableRef>().variable, src);
				}
				scriptFromOperand(dest).set(dest, std::forward<T>(src));
			}

			void release(const Operand& dest) {
				scriptFromOperand(dest).release(dest);
			 }

			ScriptExecutionOutput generateExportedVariables(std::size_t scriptIndex);

			void generate(StatementParser& parser, Generator& generator);
			bool isGenerated(std::size_t scriptIndex) const;

			const NativeFunction& getBinding(const ScriptVariableRef& bindingRef) const;

		private:
			const NodeValue* useFromCurrentEnv(const ScriptVariableRef& dest) const;

			void checkCurrentExecutionOrThrow() const {
				if (m_current == nullptr) { throw std::runtime_error("bad execution context"); }
			}
			NodeValue  getReturn();
			ExecutionContext getContext(const ScriptVariableRef& value);

			Executor& m_out;
			GenerationOutput& m_in;
			ScriptExecution* m_current = nullptr;
		};
	}
}

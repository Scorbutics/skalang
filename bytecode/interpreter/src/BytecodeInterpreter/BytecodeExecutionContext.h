#pragma once

#include <cassert>
#include "Generator/Value/BytecodeValue.h"
#include "Value/TokenVariant.h"
#include "BytecodeScriptExecution.h"

namespace ska {
	namespace bytecode {
		class GenerationOutput;
		class ExecutionContext {
		public:
			ExecutionContext(ScriptExecutionContainer& container, std::string fullScriptName, GenerationOutput& instructions) :
				m_container(container) {
				m_current = getScript(fullScriptName, instructions);
			}

			ExecutionContext(const ExecutionContext&) = delete;
			ExecutionContext& operator=(const ExecutionContext&) = delete;

			bool empty() const { return m_current->size() == 0; }

			const Instruction& currentInstruction() const { return m_current->currentInstruction(); }
			Instruction& currentInstruction() { return m_current->currentInstruction(); }

			bool incInstruction() { return m_current->incInstruction(); }

			NodeValue getCell(const Value& v) { return m_current->getCell(v); }

			void pop(NodeValue& dest) { m_current->pop(dest); }

			void pop(NodeValueArrayRaw& dest, long count) { m_current->pop(dest, count);}

			void jumpAbsolute(std::size_t value) { m_current->jumpAbsolute(value); }
			void jumpRelative(long value) { m_current->jumpRelative(value); }
			void jumpReturn() { m_current->jumpReturn(); }

			std::size_t getRelativeInstruction(long relativeValue) const {
				return m_current->getRelativeInstruction(relativeValue);
			}

			template <class ... Items>
			void push(Items&& ... items) {
				m_current->push(std::forward<Items>(items)...);
			}

			template <class T>
			T get(const Value& v) {
				return m_current->get<T>(v);
			}

			template <class T>
			void set(const Value& dest, T&& src) {
				m_current->set(dest, std::forward<T>(src));
			}

		private:
			ScriptExecution* getScript(const std::string& fullScriptName, GenerationOutput& instructions);
			ScriptExecutionContainer& m_container;
			ScriptExecution* m_current = nullptr;
		};
	}
}

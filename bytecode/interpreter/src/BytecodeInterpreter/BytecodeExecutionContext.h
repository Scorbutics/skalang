#pragma once

#include <cassert>
#include <memory>
#include "Generator/Value/BytecodeValue.h"
#include "Value/TokenVariant.h"
#include "BytecodeScriptExecution.h"

namespace ska {
	namespace bytecode {

		class ExecutionContext {
			using ScriptExecutionContainer = std::vector<std::unique_ptr<ScriptExecution>>;
		public:
			ExecutionContext(GenerationOutput& instructions) {
				m_scripts.push_back(std::make_unique<ScriptExecution>(instructions));
				m_current = &*m_scripts.back();
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
			ScriptExecutionContainer m_scripts;
			ScriptExecution* m_current = nullptr;
		};
	}
}

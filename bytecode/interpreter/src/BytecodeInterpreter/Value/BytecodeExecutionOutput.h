#pragma once
#include <memory>
#include <vector>
#include "BytecodeScriptExecution.h"

namespace ska {
	namespace bytecode {
		class ScriptGenerationOutput;
		class ExecutionOutput {
		public:
			using ScriptExecutionContainer = std::vector<std::unique_ptr<ScriptExecution>>;

			PlainMemoryTable variables;
			PlainMemoryTable callstack;

			void pop(NodeValue& dest) {
				dest = stack.back();
				stack.pop_back();
			}

			void pop(NodeValueArrayRaw& dest, long count) {
				for (auto i = 0; i < count && stack.size() > 0; i++) {
					dest.push_front(stack.back());
					stack.pop_back();
				}
			}

			ScriptExecution* script(std::size_t scriptIndex, const GenerationOutput& instructions) {
				if (scriptIndex >= scripts.size()) {
					scripts.resize(scriptIndex + 1);
					scripts[scriptIndex] = std::make_unique<ScriptExecution>(*this, instructions, scriptIndex);
				}
				return scripts[scriptIndex].get();
			}

			template <class ... Items>
			void push(Items&& ... items) {
				(pushIfNotEmpty(std::forward<decltype(items)>(items)), ...);
			}

		private:
			ScriptExecutionContainer scripts;
			PlainMemoryTable stack;

			void pushIfNotEmpty(NodeValue value) {
				if (!value.empty()) {
					stack.push_back(std::move(value));
				}
			}
		};
	}
}

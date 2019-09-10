#pragma once
#include <sstream>
#include <memory>
#include <vector>
#include "InterpreterCommandUnit.h"
#include "NodeValue/ASTNodePtr.h"
#include "BytecodeExecutionContext.h"

namespace ska {
	struct ReservedKeywordsPool;
	class TypeCrosser;

	namespace bytecode {
		class GenerationOutput;

		class Interpreter {
		public:
			using CommandInterpreter = std::vector<std::unique_ptr<InterpreterCommandUnit>>;

			Interpreter(const ReservedKeywordsPool& reserved);
			~Interpreter() = default;

			ExecutionOutput interpret(ExecutionContext& node);
			ExecutionOutput interpret(std::string fullScriptName, GenerationOutput& scripts);
		private:
			CommandInterpreter build();
			CommandInterpreter m_commandInterpreter;
			ScriptExecutionContainer m_scripts;
		};
	}
}

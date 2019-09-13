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
		class Generator;
		class GenerationOutput;

		class Interpreter {
		public:
			using CommandInterpreter = std::vector<std::unique_ptr<InterpreterCommandUnit>>;

			Interpreter(Generator& generator, const ReservedKeywordsPool& reserved);
			~Interpreter() = default;

			void interpret(ExecutionContext& node);
			ExecutionOutput interpret(std::size_t scriptIndex, GenerationOutput& scripts);
		private:
			CommandInterpreter build(Generator&);
			CommandInterpreter m_commandInterpreter;
		};
	}
}

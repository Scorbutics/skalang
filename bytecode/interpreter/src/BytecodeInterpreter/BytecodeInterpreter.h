#pragma once
#include <sstream>
#include <memory>
#include <vector>
#include "InterpreterCommandUnit.h"
#include "NodeValue/ASTNodePtr.h"
#include "BytecodeExecutionContext.h"
#include "Generator/Value/BytecodeGenerationOutput.h"

namespace ska {
	struct ReservedKeywordsPool;
	class TypeCrosser;

	namespace bytecode {
		class Generator;
		class Script;

		struct InterpretationPart {
			Script& script;
			ASTNode& node;
		};

		class Interpreter {
		public:
			using CommandInterpreter = std::vector<std::unique_ptr<InterpreterCommandUnit>>;

			Interpreter(Generator& generator, const ReservedKeywordsPool& reserved);
			~Interpreter() = default;

			void interpret(ExecutionContext& node);
			Generator& generator() { return m_generator; }
			[[nodiscard]]
			std::unique_ptr<Executor> interpret(std::size_t scriptIndex, GenerationOutput& scripts);
		private:
			CommandInterpreter build(Generator&);
			Generator& m_generator;
			CommandInterpreter m_commandInterpreter;
		};
	}
}

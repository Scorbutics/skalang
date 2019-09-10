#pragma once
#include <sstream>
#include <memory>
#include <vector>
#include "GeneratorOperatorUnit.h"
#include "NodeValue/Operator.h"
#include "NodeValue/ASTNodePtr.h"
#include "BytecodeGenerationContext.h"

namespace ska {
	struct ReservedKeywordsPool;
	class TypeCrosser;

	namespace bytecode {
		class ScriptGeneration;
		class GeneratorOperatorBase;

		class Generator {
			using OperatorGenerator = std::vector<std::unique_ptr<GeneratorOperatorUnit>>;
			friend class GeneratorOperatorBase;
		public:
			Generator(const ReservedKeywordsPool& reserved);
			~Generator() = default;

			ScriptGenerationOutput generate(GenerationContext node);
		private:
			ScriptGenerationOutput generatePart(GenerationContext node);
			ScriptGenerationOutput& postProcessing(ScriptGeneration& script, ScriptGenerationOutput& generated);
			OperatorGenerator build();

			OperatorGenerator m_operatorGenerator;
		};
	}
}

#pragma once
#include <sstream>
#include <memory>
#include <vector>
#include "GeneratorOperatorUnit.h"
#include "NodeValue/Operator.h"
#include "NodeValue/ASTNodePtr.h"
#include "BytecodeGenerationContext.h"
#include "Value/BytecodeGenerationOutput.h"
#include "Value/BytecodeScriptGenerationService.h"
#include "Value/BytecodeScriptCache.h"

namespace ska {
	struct ReservedKeywordsPool;
	class TypeCrosser;

	namespace bytecode {
		class GeneratorOperatorBase;

		class Generator {
			using OperatorGenerator = std::vector<std::unique_ptr<GeneratorOperatorUnit>>;
		public:
			Generator(const ReservedKeywordsPool& reserved);
			~Generator() = default;

			GenerationOutput generate(ScriptCache& cache, ScriptGenerationService script);
			ScriptGenerationOutput generatePart(GenerationContext node);
		private:
			GenerationOutput& postProcessing(ScriptGenerationService& script, GenerationOutput& generated);
			OperatorGenerator build();

			OperatorGenerator m_operatorGenerator;
		};
	}
}

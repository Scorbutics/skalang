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
#include "Value/BytecodeScriptGenerationOutput.h"
#include "Value/BytecodeScriptCache.h"

namespace ska {
	struct ReservedKeywordsPool;
	class TypeCrosser;
	class ASTNode;

	namespace bytecode {
		class GeneratorOperatorBase;

		class Generator {
			using OperatorGenerator = std::vector<std::unique_ptr<GeneratorOperatorUnit>>;
		public:
			Generator(const ReservedKeywordsPool& reserved);
			~Generator() = default;

			const ScriptGenerationOutput& generate(ScriptCache& cache, ScriptGenerationService script);
			const ScriptGenerationOutput& generate(ScriptCache& cache, std::size_t scriptIndex);
			ScriptGenerationOutput generatePart(GenerationContext node, std::size_t wantedId = std::numeric_limits<std::size_t>::max());

		private:
			GenerationOutput& postProcessing(ScriptGenerationService& script, GenerationOutput& generated);
			OperatorGenerator build();

			OperatorGenerator m_operatorGenerator;
		};
	}
}

#pragma once
#include <sstream>
#include <memory>
#include <vector>
#include "GeneratorOperatorUnit.h"
#include "NodeValue/Operator.h"
#include "NodeValue/ASTNodePtr.h"
#include "BytecodeGenerationContext.h"
#include "LabelReplacer.h"
#include "ObjectFieldAccessReplacer.h"

namespace ska {
	struct ReservedKeywordsPool;
	class TypeCrosser;

	namespace bytecode {
		class Script;
		class GeneratorOperatorBase;

		class Generator {
			using OperatorGenerator = std::vector<std::unique_ptr<GeneratorOperatorUnit>>;
			friend class GeneratorOperatorBase;
		public:
			Generator(const ReservedKeywordsPool& reserved);
			~Generator() = default;

			GenerationOutput generate(GenerationContext node);
		private:
			GenerationOutput generatePart(GenerationContext node);
			GenerationOutput& postProcessing(Script& script, GenerationOutput& generated);
			OperatorGenerator build();

			OperatorGenerator m_operatorGenerator;
			LabelReplacer m_labelReplacer;
			ObjectFieldAccessReplacer m_objectFieldAccessReplacer;
		};
	}
}

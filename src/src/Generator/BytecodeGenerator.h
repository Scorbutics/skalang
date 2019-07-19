#pragma once
#include <memory>
#include <vector>
#include "GeneratorOperatorUnit.h"
#include "NodeValue/Operator.h"
#include "NodeValue/ASTNodePtr.h"

#include "Interpreter/Value/ExecutionContext.h"

namespace ska {
    struct ReservedKeywordsPool;
	class Script;
	class TypeCrosser;

	class BytecodeGenerator {
		using OperatorGenerator = std::vector<std::unique_ptr<GeneratorOperatorUnit>>;
	public:
		BytecodeGenerator(const ReservedKeywordsPool& reserved, const TypeCrosser& typeCrosser);
		~BytecodeGenerator() = default;

		NodeCell generate(ExecutionContext node);
		NodeValue generate(Script& script);
	private:
		OperatorGenerator build();
		OperatorGenerator m_operatorGenerator;
		const TypeCrosser& m_typeCrosser;
	};
}

#include <iostream>

#include "BytecodeGenerator.h"

#include "NodeValue/Operator.h"
#include "NodeValue/AST.h"
#include "Interpreter/Value/Script.h"

#include "GeneratorOperatorBlock.h"
#include "GeneratorOperatorUnary.h"
#include "GeneratorOperatorVariable.h"
#include "GeneratorOperatorBinary.h"

#include "GeneratorDeclarer.h"

std::vector<std::unique_ptr<ska::GeneratorOperatorUnit>> ska::BytecodeGenerator::build() {
	auto result = std::vector<std::unique_ptr<ska::GeneratorOperatorUnit>> {};
	static constexpr auto maxOperatorEnumIndex = static_cast<std::size_t>(ska::Operator::UNUSED_Last_Length);
	result.resize(maxOperatorEnumIndex);

	GeneratorOperatorDeclare<ska::Operator::BLOCK>(*this, result);
	GeneratorOperatorDeclare<ska::Operator::UNARY>(*this, result);
	GeneratorOperatorDeclare<ska::Operator::LITERAL>(*this, result);
	GeneratorOperatorDeclare<ska::Operator::BINARY>(*this, result);
	GeneratorOperatorDeclare<ska::Operator::VARIABLE_DECLARATION>(*this, result);
	GeneratorOperatorDeclare<ska::Operator::VARIABLE_AFFECTATION>(*this, result);
	return result;
}

ska::BytecodeGenerator::BytecodeGenerator(const ReservedKeywordsPool& reserved, const TypeCrosser& typeCrosser) :
	m_operatorGenerator(build()),
	m_typeCrosser(typeCrosser) {
}

ska::BytecodeCellGroup ska::BytecodeGenerator::generate(BytecodeGenerationContext node) {
	auto& builder = m_operatorGenerator[static_cast<std::size_t>(node.pointer().op())];
	assert(builder != nullptr);
	return builder->generate(node);
}

ska::BytecodeCellGroup ska::BytecodeGenerator::generate(Script& script) {
	return generate(BytecodeGenerationContext{ script });
}

#include <iostream>

#include "BytecodeGenerator.h"

#include "NodeValue/Operator.h"
#include "NodeValue/AST.h"
#include "Interpreter/Value/Script.h"

#include "GeneratorDeclarer.h"

std::vector<std::unique_ptr<ska::BytecodeOperatorUnit>> ska::BytecodeGenerator::build() {
	auto result = std::vector<std::unique_ptr<ska::BytecodeOperatorUnit>> {};
	static constexpr auto maxOperatorEnumIndex = static_cast<std::size_t>(ska::Operator::UNUSED_Last_Length);
	result.resize(maxOperatorEnumIndex);

	//GeneratorOperatorDeclare<ska::Operator::ARRAY_USE>(*this, result);
	return result;
}

ska::BytecodeGenerator::BytecodeGenerator(const ReservedKeywordsPool& reserved, const TypeCrosser& typeCrosser) :
	m_operatorGenerator(build()),
	m_typeCrosser(typeCrosser) {
}

ska::BytecodeCell ska::BytecodeGenerator::generate(ExecutionContext node) {
	auto& builder = m_operatorGenerator[static_cast<std::size_t>(node.pointer().op())];
	assert(builder != nullptr);
	return builder->generate(node);
}

ska::BytecodeCell ska::BytecodeGenerator::generate(Script& script) {
	return interpret(ExecutionContext{ script });
}

#include "BytecodeGenerator.h"

#include "NodeValue/Operator.h"
#include "NodeValue/AST.h"
#include "Generator/Value/BytecodeScript.h"

#include "GeneratorOperatorBlock.h"
#include "GeneratorOperatorUnary.h"
#include "GeneratorOperatorVariable.h"
#include "GeneratorOperatorBinary.h"
#include "GeneratorOperatorFunction.h"
#include "GeneratorOperatorReturn.h"
#include "GeneratorOperatorArray.h"
#include "GeneratorOperatorIf.h"
#include "GeneratorOperatorFor.h"
#include "GeneratorOperatorUserDefinedObject.h"
#include "GeneratorOperatorImport.h"

#include "GeneratorDeclarer.h"

std::vector<std::unique_ptr<ska::bytecode::GeneratorOperatorUnit>> ska::bytecode::Generator::build() {
	auto result = std::vector<std::unique_ptr<GeneratorOperatorUnit>> {};
	static constexpr auto maxOperatorEnumIndex = static_cast<std::size_t>(ska::Operator::UNUSED_Last_Length);
	result.resize(maxOperatorEnumIndex);

	GeneratorOperatorDeclare<ska::Operator::BLOCK>(*this, result);
	GeneratorOperatorDeclare<ska::Operator::UNARY>(*this, result);
	GeneratorOperatorDeclare<ska::Operator::LITERAL>(*this, result);
	GeneratorOperatorDeclare<ska::Operator::BINARY>(*this, result);
	GeneratorOperatorDeclare<ska::Operator::VARIABLE_DECLARATION>(*this, result);
	GeneratorOperatorDeclare<ska::Operator::VARIABLE_AFFECTATION>(*this, result);
	GeneratorOperatorDeclare<ska::Operator::FUNCTION_PROTOTYPE_DECLARATION>(*this, result);
	GeneratorOperatorDeclare<ska::Operator::FUNCTION_DECLARATION>(*this, result);
	GeneratorOperatorDeclare<ska::Operator::FUNCTION_CALL>(*this, result);
	GeneratorOperatorDeclare<ska::Operator::RETURN>(*this, result);
	GeneratorOperatorDeclare<ska::Operator::ARRAY_DECLARATION>(*this, result);
	GeneratorOperatorDeclare<ska::Operator::ARRAY_USE>(*this, result);
	GeneratorOperatorDeclare<ska::Operator::IF>(*this, result);
	GeneratorOperatorDeclare<ska::Operator::IF_ELSE>(*this, result);
	GeneratorOperatorDeclare<ska::Operator::FOR_LOOP>(*this, result);
	GeneratorOperatorDeclare<ska::Operator::USER_DEFINED_OBJECT>(*this, result);
	GeneratorOperatorDeclare<ska::Operator::IMPORT>(*this, result);

	return result;
}

ska::bytecode::GenerationOutput& ska::bytecode::Generator::postProcessing(Script& script, GenerationOutput& generated) {
	m_labelReplacer.process(generated);
	return generated;
}

ska::bytecode::Generator::Generator(const ReservedKeywordsPool& reserved) :
	m_operatorGenerator(build()) {
}

ska::bytecode::GenerationOutput ska::bytecode::Generator::generate(GenerationContext node) {
	auto& builder = m_operatorGenerator[static_cast<std::size_t>(node.pointer().op())];
	assert(builder != nullptr);
	auto generated = builder->generate(node);
	return postProcessing(node.script(), generated);
}

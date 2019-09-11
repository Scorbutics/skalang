#include "Config/LoggerConfigLang.h"
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
#include "GeneratorOperatorFieldAccess.h"

#include "GeneratorDeclarer.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::bytecode::Generator);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::Generator)
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::Generator)

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
	GeneratorOperatorDeclare<ska::Operator::FIELD_ACCESS>(*this, result);
	GeneratorOperatorDeclare<ska::Operator::IMPORT>(*this, result);

	return result;
}

ska::bytecode::GenerationOutput& ska::bytecode::Generator::postProcessing(ScriptGenerationService& script, GenerationOutput& generated) {
	return generated;
}

ska::bytecode::Generator::Generator(const ReservedKeywordsPool& reserved) :
	m_operatorGenerator(build()) {
}

ska::bytecode::ScriptGenerationOutput ska::bytecode::Generator::generatePart(GenerationContext node) {
	const auto& operatorNode = node.pointer().op();
	LOG_DEBUG << "Accessing operator " << operatorNode;
	auto& builder = m_operatorGenerator[static_cast<std::size_t>(operatorNode)];
	assert(builder != nullptr);
	return builder->generate(node);
}

ska::bytecode::GenerationOutput ska::bytecode::Generator::generate(ScriptGenerationService script) {
	auto container = GenerationOutput { };
	auto index = container.push(std::move(script));
	auto service = container.backService().program().name();
	container.setOut(index, generatePart(GenerationContext{ container }));
	auto& out = container; //postProcessing(service, container);

	LOG_INFO << "Final generation " << out.back() << " for script " << service;

	return std::move(out);
}

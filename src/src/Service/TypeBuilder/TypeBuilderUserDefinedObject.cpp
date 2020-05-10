#include "Config/LoggerConfigLang.h"
#include <fstream>
#include "TypeBuilderUserDefinedObject.h"

#include "NodeValue/AST.h"
#include "NodeValue/ScriptAST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::USER_DEFINED_OBJECT>)
SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::RETURN>)

ska::TypeHierarchy ska::TypeBuilderOperator<ska::Operator::USER_DEFINED_OBJECT>::build(const ScriptAST& script, OperateOn node) {
	auto* symbol = script.symbols().enclosingType();
	assert(symbol == nullptr || symbol->type() == ExpressionType::FUNCTION);
	return { ska::Type::MakeCustom<ExpressionType::OBJECT>(symbol), symbol };
}

ska::TypeHierarchy ska::TypeBuilderOperator<ska::Operator::RETURN>::build(const ScriptAST& script, OperateOn node) {
	return { node.GetValue().type().value(), node.GetValue().symbol() };
}

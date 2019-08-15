#include <fstream>
#include "TypeBuilderUserDefinedObject.h"

#include "NodeValue/AST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::USER_DEFINED_OBJECT>)

ska::Type ska::TypeBuilderOperator<ska::Operator::USER_DEFINED_OBJECT>::build(const ScriptAST& script, OperateOn node) {
	return ska::Type::MakeCustom<ExpressionType::OBJECT>(nullptr);
}

#include <fstream>
#include "TypeBuilderReturn.h"

#include "NodeValue/AST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::RETURN>)

ska::Type ska::TypeBuilderOperator<ska::Operator::RETURN>::build(const Script& script, OperateOn node) {
	return node.GetValue().type().value();
}

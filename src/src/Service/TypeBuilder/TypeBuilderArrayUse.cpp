#include "Config/LoggerConfigLang.h"
#include "TypeBuilderArrayUse.h"

#include "TypeBuilderCalculatorDispatcher.h"
#include "Service/SymbolTable.h"
#include "NodeValue/AST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::ARRAY_USE>);

ska::Type ska::TypeBuilderOperator<ska::Operator::ARRAY_USE>::build(Parser& parser, const SymbolTable& symbols, const ASTNode& node) {
    assert(node.size() == 2 && node[0].type().has_value() && node[0].type().value().compound().size() == 1);
	return node[0].type().value().compound()[0];
}

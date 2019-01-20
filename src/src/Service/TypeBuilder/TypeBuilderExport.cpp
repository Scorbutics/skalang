#include <fstream>
#include "TypeBuilderExport.h"

#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"
#include "Service/StatementParser.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::EXPORT>)

ska::Type ska::TypeBuilderOperator<ska::Operator::EXPORT>::build(StatementParser& parser, const SymbolTable& symbols, const ASTNode& node) {
	assert(node.size() == 1);
	return node[0].type().value();
}

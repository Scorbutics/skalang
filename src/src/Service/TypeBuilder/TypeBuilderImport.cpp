#include <fstream>
#include "TypeBuilderImport.h"

#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"
#include "Service/StatementParser.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::IMPORT>)

ska::Type ska::TypeBuilderOperator<ska::Operator::IMPORT>::build(StatementParser& parser, const SymbolTable& symbols, const ASTNode& node) {
	assert(node.size() == 3 && !symbols.nested().empty());
	auto importName = "import " + node[0].name();
	auto importType = Type{ importName, ExpressionType::OBJECT, *symbols.nested().back() };
	return importType;
}

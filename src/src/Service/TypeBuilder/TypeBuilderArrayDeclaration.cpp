#include "TypeBuilderArrayDeclaration.h"

#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"
#include "TypeBuilderCalculatorDispatcher.h"

ska::Type ska::TypeBuilderOperator<ska::Operator::ARRAY_DECLARATION>::build(Parser& parser, const SymbolTable& symbols, const ASTNode& node) {
	auto type = Type{ ExpressionType::ARRAY };
	if (node.size() == 0) {
		return type;
	}
	const auto& subType = node[0].type().value();
	type.add(subType);
	return type;
}

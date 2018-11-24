#include "TypeBuilderBinary.h"

#include "AST.h"
#include "TypeBuilderCalculatorDispatcher.h"

ska::Type ska::TypeBuilderOperator<ska::Operator::BINARY>::build(const SymbolTable& symbols, const ASTNode& node) {
    assert(node.size() == 2 && !node.name().empty());
    const auto& type1 = node[0].type().value();
    const auto& type2 = node[1].type().value();
	return Type{ type1.crossTypes(node.name()[0], type2) };
}

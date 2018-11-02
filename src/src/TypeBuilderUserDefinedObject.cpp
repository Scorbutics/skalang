#include "TypeBuilderUserDefinedObject.h"

#include "AST.h"
#include "SymbolTable.h"
#include "TypeBuilderCalculatorDispatcher.h"

ska::Type ska::TypeBuilderOperator<ska::Operator::USER_DEFINED_OBJECT>::build(const SymbolTable& symbols, ASTNode& node) {
    assert(node.size() == 1);
    return TypeBuilderDispatchCalculation(symbols, node[0]);
}

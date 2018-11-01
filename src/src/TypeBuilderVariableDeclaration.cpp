#include "TypeBuilderVariableDeclaration.h"

#include "AST.h"
#include "SymbolTable.h"
#include "TypeBuilderCalculatorDispatcher.h"

ska::Type ska::TypeBuilderOperator<ska::Operator::VARIABLE_DECLARATION>::build(const SymbolTable& symbols, ASTNode& node) {
    assert(node.size() == 1);
    const auto typeStr = node[0].asString();
    
    if (ExpressionTypeMap.find(typeStr) == ExpressionTypeMap.end()) {
        const auto symbolType = symbols[typeStr];
        if (symbolType == nullptr) {
            throw std::runtime_error("unknown type detected as function parameter : " + node[0].asString());
        }
        return (node[0].type() = symbolType->getType()).value();
    } else {
        TypeBuilderDispatchCalculation(symbols, node[0]);
        return ExpressionTypeMap.at(node[0].asString());
    }
}

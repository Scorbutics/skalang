#include "TypeBuilderFunctionDeclaration.h"

#include "TypeBuilderCalculatorDispatcher.h"
#include "SymbolTable.h"
#include "AST.h"

ska::Type ska::TypeBuilderOperator<ska::Operator::FUNCTION_DECLARATION>::build(const SymbolTable& symbols, ASTNode& node) {
    auto functionType = Type{ ExpressionType::FUNCTION };
    for (auto index = 0u; index < node.size(); index++) {
        auto varType = TypeBuilderDispatchCalculation(symbols, node[index]);
        if(varType == ExpressionType::OBJECT) {
            varType.name(node.asString());
        }
        functionType.add(std::move(varType));
    }
#ifdef SKALANG_LOG_TYPE_BUILDER_OPERATOR
    std::cout << "function declaration \""<< node.asString() <<"\" with type "<< functionType.asString() << std::endl;
#endif
    return functionType;
}

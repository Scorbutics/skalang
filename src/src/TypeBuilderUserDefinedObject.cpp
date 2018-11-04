#include "TypeBuilderUserDefinedObject.h"

#include "AST.h"
#include "SymbolTable.h"
#include "TypeBuilderCalculatorDispatcher.h"

ska::Type ska::TypeBuilderOperator<ska::Operator::USER_DEFINED_OBJECT>::build(const SymbolTable& symbols, ASTNode& node) {
    //TODO : cette spécialisation est a supprimer (n'a pas de sens, un return n'a pas de type)
	//assert(node.size() == 1);
    //return TypeBuilderDispatchCalculation(symbols, node[0]);
	return ExpressionType::VOID;
}

#include "TypeBuilderFilterParameterDeclaration.h"

#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"
#include "NodeValue/ScriptAST.h"

ska::TypeHierarchy ska::TypeBuilderOperator<ska::Operator::FILTER_PARAMETER_DECLARATION>::build(const ScriptAST& script, OperateOn node) {
	auto collectionType = node.GetCollectionType();

	if (collectionType == ExpressionType::VOID) {
		return Type::MakeBuiltIn<ExpressionType::INT>();
	} 
	
	if (collectionType == ExpressionType::ARRAY) {
		return collectionType[0];
	}

	auto ss = std::stringstream{};
	ss << "invalid collection for the current filter : expected an array but got \"" << collectionType << "\" instead";
	throw std::runtime_error(ss.str());
}

#include "TypeBuilderFactoryPrototypeDeclaration.h"

#include "NodeValue/AST.h"
#include "NodeValue/ScriptAST.h"

ska::TypeHierarchy ska::TypeBuilderOperator<ska::Operator::FACTORY_PROTOTYPE_DECLARATION>::build(const ScriptAST& script, OperateOn node) {
	//Access private function factory prototype declaration to obtain real factory type (excluding return type)
	auto& functionParametersListNode = node.GetFunctionParametersList();

	auto result = Type::MakeCustom<ExpressionType::FUNCTION>(nullptr);
	for (const auto& parameterNode : functionParametersListNode) {
		result.add(node.GetPrivateFunctionFactoryField(parameterNode->name())->type());
	}

	result.add(node.GetFunctionReturnType());

	return { result, script.symbols()[node.GetFunctionName()] };
}

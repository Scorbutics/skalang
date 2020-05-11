#include "TypeBuilderFactoryPrototypeDeclaration.h"

#include "NodeValue/AST.h"
#include "NodeValue/ScriptAST.h"

ska::TypeHierarchy ska::TypeBuilderOperator<ska::Operator::FACTORY_PROTOTYPE_DECLARATION>::build(const ScriptAST& script, OperateOn node) {
	//Access private function factory prototype declaration to obtain real factory type (excluding return type)
	auto& functionParametersListNode = node.GetFunctionParametersList();

	auto result = Type::MakeCustom<ExpressionType::FUNCTION>(nullptr);
	auto index = std::size_t{ 0 };
	for (const auto& parameterNode : functionParametersListNode) {
		if (index != node.GetFunctionParametersSize()) {
			result.add(node.GetPrivateFunctionFactoryField(parameterNode->name())->type());
		}
		index++;
	}

	result.add(node.GetFunctionReturnType());

	return { result, script.symbols()[node.GetFunctionName()] };
}

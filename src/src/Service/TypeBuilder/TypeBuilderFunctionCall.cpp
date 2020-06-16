#include "Config/LoggerConfigLang.h"
#include "TypeBuilderFunctionCall.h"

#include "TypeBuilderCalculatorDispatcher.h"
#include "Service/SymbolTable.h"
#include "NodeValue/ScriptAST.h"
#include "NodeValue/AST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::FUNCTION_CALL>);
SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::FUNCTION_MEMBER_CALL>);

template <class OperateOn>
ska::TypeHierarchy CommonBuild(const ska::ScriptAST& script, OperateOn& node) {
	auto type = node.GetFunctionType();
	auto* symbol = node.GetFunctionNameNode().symbol();
    
	if (type.size() == 0) {
        throw std::runtime_error("Unable to get function return type");
    }
	
	return { type.back(), symbol };
}

ska::TypeHierarchy ska::TypeBuilderOperator<ska::Operator::FUNCTION_CALL>::build(const ScriptAST& script, OperateOn node) {
	return CommonBuild(script, node);
}

ska::TypeHierarchy ska::TypeBuilderOperator<ska::Operator::FUNCTION_MEMBER_CALL>::build(const ScriptAST& script, OperateOn node) {
	return CommonBuild(script, node);
}
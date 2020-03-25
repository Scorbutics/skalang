#include "Config/LoggerConfigLang.h"
#include "TypeBuilderFunctionCall.h"

#include "TypeBuilderCalculatorDispatcher.h"
#include "Service/SymbolTable.h"
#include "NodeValue/ScriptAST.h"
#include "NodeValue/AST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::FUNCTION_CALL>);

ska::TypeHierarchy ska::TypeBuilderOperator<ska::Operator::FUNCTION_CALL>::build(const ScriptAST& script, OperateOn node) {
	auto type = node.GetFunctionType();
	auto* symbol = node.GetFunctionSymbol();
	SLOG(LogLevel::Info) << "function " << type << " call";
    
	if (type.size() == 0) {
        throw std::runtime_error("Unable to get function return type");
    }
	
	return { type.back(), symbol };
}

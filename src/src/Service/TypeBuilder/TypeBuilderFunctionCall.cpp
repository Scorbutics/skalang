#include "Config/LoggerConfigLang.h"
#include "TypeBuilderFunctionCall.h"

#include "TypeBuilderCalculatorDispatcher.h"
#include "Service/SymbolTable.h"
#include "NodeValue/AST.h"
#include "Operation/OperationFunctionDeclaration.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::TypeBuilderOperator<ska::Operator::FUNCTION_CALL>);

ska::Type ska::TypeBuilderOperator<ska::Operator::FUNCTION_CALL>::build(const SymbolTable& symbols, OperateOn node) {
	const auto type = node.GetFunctionType();
	SLOG(LogLevel::Info) << "function " << type << " call";
    if(type.compound().size() == 0) {
        throw std::runtime_error("Unable to get function return type");
    }
	return type.compound().back();
}

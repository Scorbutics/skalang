#include "Config/LoggerConfigLang.h"
#include "TypeBuilderArrayUse.h"

#include "TypeBuilderCalculatorDispatcher.h"
#include "Service/SymbolTable.h"
#include "NodeValue/AST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::ARRAY_USE>);

ska::Type ska::TypeBuilderOperator<ska::Operator::ARRAY_USE>::build(const SymbolTable& symbols, OperateOn node) {
    return node.GetArraySubType();
}

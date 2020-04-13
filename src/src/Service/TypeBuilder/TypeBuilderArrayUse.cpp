#include "Config/LoggerConfigLang.h"
#include "TypeBuilderArrayUse.h"

#include "TypeBuilderCalculatorDispatcher.h"
#include "Service/SymbolTable.h"
#include "NodeValue/AST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::ARRAY_USE>);

ska::TypeHierarchy ska::TypeBuilderOperator<ska::Operator::ARRAY_USE>::build(const ScriptAST& script, OperateOn node) {
    return node.GetArraySubType();
}

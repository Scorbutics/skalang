#include "Config/LoggerConfigLang.h"
#include "TypeBuilderVariableDeclaration.h"

#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"
#include "NodeValue/ScriptAST.h"
#include "TypeBuilderCalculatorDispatcher.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::VARIABLE_AFFECTATION>);

ska::TypeHierarchy ska::TypeBuilderOperator<ska::Operator::VARIABLE_AFFECTATION>::build(const ScriptAST& script, OperateOn node) {
    const auto* symbol = script.symbols()[node.GetVariableName()];
    if (symbol != nullptr && symbol->type() != ExpressionType::VOID) {
	SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::VARIABLE_AFFECTATION>) << "%02cVariable declaration has already type \"" << symbol->type() << "\"";
	    return symbol->type();
    }

    auto resultType = node.GetVariableValueNode().type().value();
    const Symbol* symbolLink = ExpressionTypeIsBuiltIn(resultType.type()) ? nullptr : node.GetVariableValueNode().symbol();
    SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::VARIABLE_AFFECTATION>) << "%02cVariable declaration type \"" << resultType << "\"" << (symbolLink == nullptr ? "" : (" for symbol \"" + symbolLink->name() + "\""));
    return { std::move(resultType), std::move(symbolLink) };
}

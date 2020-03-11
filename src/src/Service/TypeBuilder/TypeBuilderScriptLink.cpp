#include "TypeBuilderScriptLink.h"
#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"
#include "NodeValue/ScriptAST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::SCRIPT_LINK>)

ska::TypeHierarchy ska::TypeBuilderOperator<ska::Operator::SCRIPT_LINK>::build(ScriptAST& script, OperateOn node) {
	auto& symbols = script.symbols();
	const auto* symbol = symbols[node.GetValue().name()];
    SLOG(LogLevel::Info) << "Looking for script cache \"" << node.GetValue().name() << "\" with symbol " << (symbol == nullptr ? "" : symbol->name());
	return symbol == nullptr ? Type::MakeBuiltIn(ExpressionType::VOID) : Type::MakeCustom<ExpressionType::OBJECT>(symbol);
}

#include "TypeBuilderScriptLink.h"
#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"
#include "Service/Script.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::TypeBuilderOperator<ska::Operator::SCRIPT_LINK>)

ska::Type ska::TypeBuilderOperator<ska::Operator::SCRIPT_LINK>::build(const Script& script, OperateOn node) {
	auto& symbols = script.symbols();
	const auto* symbol = symbols[node.GetValue().name()];
    SLOG(LogLevel::Info) << "Looking for script cache \"" << node.GetValue().name() << "\" with symbol " << (symbol == nullptr ? "" : symbol->getName());
	return symbol == nullptr ? Type::MakeBuiltIn(ExpressionType::VOID) : Type::MakeCustom<ExpressionType::OBJECT>(symbol);
}

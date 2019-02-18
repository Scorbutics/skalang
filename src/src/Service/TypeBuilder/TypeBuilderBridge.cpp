#include "TypeBuilderBridge.h"
#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::TypeBuilderOperator<ska::Operator::BRIDGE>)

ska::Type ska::TypeBuilderOperator<ska::Operator::BRIDGE>::build(const SymbolTable& symbols, OperateOn node) {
    const auto* symbol = symbols[node.GetValue().name()];
    SLOG(LogLevel::Info) << "Bridging script " << node.GetValue().name() << " with symbol " << (symbol == nullptr ? "" : symbol->getName());
	return symbol == nullptr ? Type::MakeBuiltIn(ExpressionType::VOID) : Type::MakeCustom<ExpressionType::OBJECT>(symbol);
}

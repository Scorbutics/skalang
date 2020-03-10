#include <fstream>
#include "TypeBuilderImport.h"

#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"
#include "Service/StatementParser.h"
#include "NodeValue/ScriptAST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::IMPORT>)

ska::Type ska::TypeBuilderOperator<ska::Operator::IMPORT>::build(const ScriptAST& script, OperateOn node) {
    SLOG(LogLevel::Info) << "Importing script " << node.GetScriptPathNode().name();
	auto& symbols = script.symbols();
	assert(symbols.countDirect() > 0);
	const auto*	symbol = symbols.lookup(SymbolTableLookup::hierarchical(node.GetScriptPathNode().name()), SymbolTableNested::lastChild());
	return Type::MakeCustom<ExpressionType::OBJECT>(symbol);
}

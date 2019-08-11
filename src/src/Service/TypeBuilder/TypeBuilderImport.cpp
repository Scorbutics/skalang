#include <fstream>
#include "TypeBuilderImport.h"

#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"
#include "Service/StatementParser.h"
#include "Interpreter/Value/Script.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::IMPORT>)

ska::Type ska::TypeBuilderOperator<ska::Operator::IMPORT>::build(const Script& script, OperateOn node) {
    SLOG(LogLevel::Info) << "Importing script " << node.GetScriptPathNode().name();
	auto& symbols = script.symbols();
	assert(!symbols.current()->children().empty());
	auto& s = *symbols.current()->children().back();
	const auto*	symbol = s[node.GetScriptPathNode().name()];
	return Type::MakeCustom<ExpressionType::OBJECT>(symbol);
}

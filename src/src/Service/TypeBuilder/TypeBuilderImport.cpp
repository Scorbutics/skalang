#include <fstream>
#include "TypeBuilderImport.h"

#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"
#include "Service/StatementParser.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::TypeBuilderOperator<ska::Operator::IMPORT>)

ska::Type ska::TypeBuilderOperator<ska::Operator::IMPORT>::build(const SymbolTable& symbols, OperateOn node) {
	//TODO à remplacer avec return node.GetScriptRootType(); avec GetScriptRootType = node.script().type().value();

    SLOG(LogLevel::Info) << "Importing script as " << node.GetImportVariableName() << " with script path " << node.GetScriptPath();
    const auto* symbol = symbols[node.GetScriptPath()];	
	return Type::MakeCustom<ExpressionType::OBJECT>(symbol);
}

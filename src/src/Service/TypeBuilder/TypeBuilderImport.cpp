#include <fstream>
#include "TypeBuilderImport.h"

#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"
#include "Service/StatementParser.h"

SKA_LOGC_CONFIG(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::IMPORT>)

ska::Type ska::TypeBuilderOperator<ska::Operator::IMPORT>::build(const SymbolTable& symbols, OperateOn node) {
	//assert(!symbols.nested().empty());
	
    SLOG(LogLevel::Info) << "Importing script as " << node.GetImportVariableName();
    auto importType = Type::MakeCustom<ExpressionType::OBJECT>(nullptr);
	return importType;
}

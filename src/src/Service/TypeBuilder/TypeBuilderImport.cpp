#include <fstream>
#include "TypeBuilderImport.h"

#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"
#include "Service/StatementParser.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::IMPORT>)

ska::Type ska::TypeBuilderOperator<ska::Operator::IMPORT>::build(const SymbolTable& symbols, OperateOn node) {
    SLOG(LogLevel::Info) << "Importing script as " << node.GetImportVariableName();
    return Type::MakeCustom<ExpressionType::OBJECT>(nullptr);
}

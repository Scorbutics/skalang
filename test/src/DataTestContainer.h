#pragma once

#include <memory>
#include "Service/SymbolTable.h"
#include "Service/StatementParser.h"
#include "Service/SemanticTypeChecker.h"
#include "Service/TypeBuilder/TypeBuilder.h"
#include "Service/SymbolTableTypeUpdater.h"
#include "Service/ReservedKeywordsPool.h"
#include "Interpreter/Interpreter.h"

struct DataTestContainer {
    using ParserPtr = std::unique_ptr<ska::StatementParser>;
    using SemanticTypeCheckerPtr = std::unique_ptr<ska::SemanticTypeChecker>;
    using TypeBuilderPtr = std::unique_ptr<ska::TypeBuilder>;
	using SymbolTableTypeUpdaterPtr = std::unique_ptr<ska::SymbolTableTypeUpdater>;
	using InterpreterPtr = std::unique_ptr<ska::Interpreter>;

	ska::ReservedKeywordsPool reservedKeywords;
	InterpreterPtr interpreter;
    ParserPtr parser;
    SemanticTypeCheckerPtr typeChecker;
    TypeBuilderPtr typeBuilder;
	SymbolTableTypeUpdaterPtr symbolsTypeUpdater;
};

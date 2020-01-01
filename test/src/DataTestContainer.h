#pragma once

#include <memory>
#include "Service/SymbolTable.h"
#include "Service/StatementParser.h"
#include "Service/SemanticTypeChecker.h"
#include "Service/TypeBuilder/TypeBuilder.h"
#include "Service/TypeBuilder/TypeBuildUnit.h"
#include "Service/SymbolTableUpdater.h"
#include "Service/ReservedKeywordsPool.h"

struct DataTestContainer {
    using ParserPtr = std::unique_ptr<ska::StatementParser>;
    using SemanticTypeCheckerPtr = std::unique_ptr<ska::SemanticTypeChecker>;
    using TypeBuilderPtr = std::unique_ptr<ska::TypeBuilder>;
	using SymbolTableUpdaterPtr = std::unique_ptr<ska::SymbolTableUpdater>;

	ska::ReservedKeywordsPool reservedKeywords;
    ParserPtr parser;
    SemanticTypeCheckerPtr typeChecker;
    TypeBuilderPtr typeBuilder;
	SymbolTableUpdaterPtr symbolsTypeUpdater;
};

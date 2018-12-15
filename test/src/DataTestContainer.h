#pragma once

#include <memory>
#include "Service/SymbolTable.h"
#include "Service/Parser.h"
#include "Service/SemanticTypeChecker.h"
#include "TypeBuilder/TypeBuilder.h"
#include "Service/SymbolTableTypeUpdater.h"

struct DataTestContainer {
    using SymbolTablePtr = std::unique_ptr<ska::SymbolTable>;
    using ParserPtr = std::unique_ptr<ska::Parser>;
    using SemanticTypeCheckerPtr = std::unique_ptr<ska::SemanticTypeChecker>;
    using TypeBuilderPtr = std::unique_ptr<ska::TypeBuilder>;
	using SymbolTableTypeUpdaterPtr = std::unique_ptr<ska::SymbolTableTypeUpdater>;

    ParserPtr parser;
    SemanticTypeCheckerPtr typeChecker;
    SymbolTablePtr symbols;
    TypeBuilderPtr typeBuilder;
	SymbolTableTypeUpdaterPtr symbolsTypeUpdater;
};

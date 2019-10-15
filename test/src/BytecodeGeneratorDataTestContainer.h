#pragma once

#include <memory>
#include "Service/SymbolTable.h"
#include "Service/StatementParser.h"
#include "Service/SemanticTypeChecker.h"
#include "Service/TypeBuilder/TypeBuilder.h"
#include "Service/TypeBuilder/TypeBuildUnit.h"
#include "Service/SymbolTableUpdater.h"
#include "Service/ReservedKeywordsPool.h"
#include "Generator/BytecodeGenerator.h"
#include "Generator/Value/BytecodeScriptStorage.h"

struct BytecodeGeneratorDataTestContainer {
    using ParserPtr = std::unique_ptr<ska::StatementParser>;
    using SemanticTypeCheckerPtr = std::unique_ptr<ska::SemanticTypeChecker>;
    using TypeBuilderPtr = std::unique_ptr<ska::TypeBuilder>;
	using SymbolTableUpdaterPtr = std::unique_ptr<ska::SymbolTableUpdater>;
	using BytecodeGeneratorPtr = std::unique_ptr<ska::bytecode::Generator>;
	
	ska::ReservedKeywordsPool reservedKeywords;
	BytecodeGeneratorPtr generator;
    ParserPtr parser;
    SemanticTypeCheckerPtr typeChecker;
    TypeBuilderPtr typeBuilder;
	SymbolTableUpdaterPtr symbolsTypeUpdater;
    ska::bytecode::ScriptStorage storage;
};

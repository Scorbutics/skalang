#pragma once

#include <memory>
#include "Service/SymbolTable.h"
#include "Service/StatementParser.h"
#include "Service/SemanticTypeChecker.h"
#include "Service/TypeBuilder/TypeBuilder.h"
#include "Service/TypeBuilder/TypeBuildUnit.h"
#include "Service/ReservedKeywordsPool.h"

#include "Interpreter/Interpreter.h"

struct InterpreterDataTestContainer {
	using ParserPtr = std::unique_ptr<ska::StatementParser>;
	using SemanticTypeCheckerPtr = std::unique_ptr<ska::SemanticTypeChecker>;
	using TypeBuilderPtr = std::unique_ptr<ska::TypeBuilder>;
	using InterpreterPtr = std::unique_ptr<ska::Interpreter>;

	ska::ReservedKeywordsPool reservedKeywords;
	ParserPtr parser;
	SemanticTypeCheckerPtr typeChecker;
	TypeBuilderPtr typeBuilder;
	InterpreterPtr interpreter;
};

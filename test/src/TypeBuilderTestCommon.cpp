#include "TypeBuilderTestCommon.h"
#include "Service/Tokenizer.h"
#include "Service/ReservedKeywordsPool.h"
#include "Service/StatementParser.h"
#include "Service/SymbolTable.h"
#include "Service/TypeCrosser/TypeCrossExpression.h"

ska::ScriptAST TypeBuilderTestCommonBuildAST(std::unordered_map<std::string, ska::ScriptHandleASTPtr>& scriptCache, const std::string& input, DataTestContainer& data, bool buildType) {
	auto tokenizer = ska::Tokenizer{ data.reservedKeywords, input };
	const auto tokens = tokenizer.tokenize();
	auto reader = ska::ScriptAST{scriptCache, "main", tokens };
	auto typeCrosser = ska::TypeCrosser{};

	data.parser = std::make_unique<ska::StatementParser>(data.reservedKeywords);
	//data.symbols = std::make_unique<ska::SymbolTable>(*data.parser);
	if (buildType) {
		data.typeBuilder = std::make_unique<ska::TypeBuilder>(*data.parser, typeCrosser);
		data.symbolsTypeUpdater = std::make_unique<ska::SymbolTableUpdater>(*data.parser);
	}
	return reader;
}

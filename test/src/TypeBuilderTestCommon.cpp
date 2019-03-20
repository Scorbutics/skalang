#include "TypeBuilderTestCommon.h"
#include "Service/Tokenizer.h"
#include "Service/ReservedKeywordsPool.h"
#include "Service/StatementParser.h"
#include "Service/SymbolTable.h"

ska::Script TypeBuilderTestCommonBuildAST(std::unordered_map<std::string, ska::ScriptHandlePtr>& scriptCache, const std::string& input, DataTestContainer& data, bool buildType) {
	auto tokenizer = ska::Tokenizer{ data.reservedKeywords, input };
	const auto tokens = tokenizer.tokenize();
	auto reader = ska::Script{scriptCache, "main", tokens };

	data.parser = std::make_unique<ska::StatementParser>(data.reservedKeywords);
	//data.symbols = std::make_unique<ska::SymbolTable>(*data.parser);
	if (buildType) {
		data.typeBuilder = std::make_unique<ska::TypeBuilder>(*data.parser);
		data.symbolsTypeUpdater = std::make_unique<ska::SymbolTableTypeUpdater>(*data.parser);
	}
	return reader;
}

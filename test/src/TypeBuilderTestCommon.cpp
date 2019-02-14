#include "TypeBuilderTestCommon.h"
#include "Service/Tokenizer.h"
#include "Service/ReservedKeywordsPool.h"
#include "Service/StatementParser.h"
#include "Service/SymbolTable.h"
#include "Service/Script.h"

std::unique_ptr<ska::ASTNode> TypeBuilderTestCommonBuildAST(const std::string& input, DataTestContainer& data, bool buildType) {
	const auto reservedKeywords = ska::ReservedKeywordsPool{};
	auto tokenizer = ska::Tokenizer{ reservedKeywords, input };
	const auto tokens = tokenizer.tokenize();
	auto reader = ska::Script{ tokens };

	data.parser = std::make_unique<ska::StatementParser>(reservedKeywords);
	data.symbols = std::make_unique<ska::SymbolTable>(*data.parser);
	if (buildType) {
		data.typeBuilder = std::make_unique<ska::TypeBuilder>(*data.parser, *data.symbols);
		data.symbolsTypeUpdater = std::make_unique<ska::SymbolTableTypeUpdater>(*data.parser, *data.symbols);
	}
	return data.parser->parse(reader);
}

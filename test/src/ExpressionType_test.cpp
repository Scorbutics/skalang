#include <doctest.h>
#include "Config/LoggerConfigLang.h"
#include "NodeValue/ExpressionType.h"
#include "Service/Tokenizer.h"
#include "Service/SymbolTable.h"
#include "Service/StatementParser.h"
#include "Service/Script.h"

using SymbolTablePtr = std::unique_ptr<ska::SymbolTable>;
using ParserPtr = std::unique_ptr<ska::StatementParser>;

std::unique_ptr<ska::ASTNode> ASTFromInputSemanticExpressionType(const std::string& input, ParserPtr& parser_test, SymbolTablePtr& table_test) {
	static auto refCounter = 0;
    const auto reservedKeywords = ska::ReservedKeywordsPool{};
	auto tokenizer = ska::Tokenizer { reservedKeywords, input };
	const auto tokens = tokenizer.tokenize();
	auto reader = ska::Script { "main", tokens };
	parser_test = std::make_unique<ska::StatementParser> ( reservedKeywords );
    table_test = std::make_unique<ska::SymbolTable> (*parser_test);
    auto result = reader.parse(*parser_test);
    ska::Script::clearCache();
    return result;
}

TEST_CASE("[ExpressionType]") {
    
    ParserPtr parser_test;
    SymbolTablePtr symbol_test;
    auto astPtr = ASTFromInputSemanticExpressionType("{var toto = 2;}", parser_test, symbol_test);
    auto& table = *symbol_test->nested()[0];
	auto& nested = table.createNested();

	SUBCASE("Type is set") {
		auto type = ska::Type::MakeCustom<ska::ExpressionType::FUNCTION>((*symbol_test)["toto"]);
        CHECK(type == ska::ExpressionType::FUNCTION);

		auto type2 = ska::Type::MakeBuiltIn<ska::ExpressionType::FLOAT>();
		CHECK(type2 == ska::ExpressionType::FLOAT);
    }

    SUBCASE("Type Copy") {
		auto type = ska::Type::MakeCustom<ska::ExpressionType::OBJECT>((*symbol_test)["toto"]);
		type.add(ska::Type::MakeBuiltIn<ska::ExpressionType::INT>());
        auto typeCopied = type;
        CHECK(typeCopied == ska::ExpressionType::OBJECT);
        CHECK(!typeCopied.compound().empty());
        CHECK(typeCopied.compound()[0] == ska::ExpressionType::INT);
        CHECK(typeCopied == type);
    }

    SUBCASE("Type Move") {
		auto type = ska::Type::MakeCustom<ska::ExpressionType::OBJECT>((*symbol_test)["toto"]);
		type.add(ska::Type::MakeBuiltIn<ska::ExpressionType::INT>());
        auto typeMoved = std::move(type);        
        CHECK(typeMoved == ska::ExpressionType::OBJECT);
        CHECK(!typeMoved.compound().empty());
        CHECK(typeMoved.compound()[0] == ska::ExpressionType::INT);
    }
}

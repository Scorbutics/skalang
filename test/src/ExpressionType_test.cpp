#include <doctest.h>
#include "LoggerConfigLang.h"
#include "ExpressionType.h"
#include "Tokenizer.h"
#include "SymbolTable.h"
#include "Parser.h"

using SymbolTablePtr = std::unique_ptr<ska::SymbolTable>;
using ParserPtr = std::unique_ptr<ska::Parser>;

std::unique_ptr<ska::ASTNode> ASTFromInputSemanticExpressionType(const std::string& input, ParserPtr& parser_test, SymbolTablePtr& table_test) {
	const auto reservedKeywords = ska::ReservedKeywordsPool{};
	auto tokenizer = ska::Tokenizer { reservedKeywords, input };
	const auto tokens = tokenizer.tokenize();
	auto reader = ska::TokenReader { tokens };
	parser_test = std::make_unique<ska::Parser> ( reservedKeywords, reader );
    table_test = std::make_unique<ska::SymbolTable> (*parser_test);
    return parser_test->parse();
}

TEST_CASE("[ExpressionType]") {
    
    ParserPtr parser_test;
    SymbolTablePtr symbol_test;
    auto astPtr = ASTFromInputSemanticExpressionType("{var toto = 2;}", parser_test, symbol_test);
    auto& table = *symbol_test->nested()[0];
    
    SUBCASE("Type is set") {
        auto type = ska::Type { "toto", ska::ExpressionType::OBJECT };
        CHECK(type.getName() == "toto");
        CHECK(type == ska::ExpressionType::OBJECT);
    }

    SUBCASE("Type Copy") {
        auto type = ska::Type { "toto", ska::ExpressionType::OBJECT };
        type.add(ska::ExpressionType::INT);
        auto typeCopied = type;
        CHECK(typeCopied.getName() == "toto");
        CHECK(typeCopied == ska::ExpressionType::OBJECT);
        CHECK(!typeCopied.compound().empty());
        CHECK(typeCopied.compound()[0] == ska::ExpressionType::INT);
        CHECK(typeCopied == type);
    }

    SUBCASE("Type Move") {
        auto type = ska::Type { "toto", ska::ExpressionType::OBJECT };
        type.add(ska::ExpressionType::INT);
        auto typeMoved = std::move(type);
        CHECK(typeMoved.getName() == "toto");
        CHECK(typeMoved == ska::ExpressionType::OBJECT);
        CHECK(!typeMoved.compound().empty());
        CHECK(typeMoved.compound()[0] == ska::ExpressionType::INT);
        CHECK(type.asString() == "INVALID_MOVED");
    }
}

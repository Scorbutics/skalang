#include <doctest.h>
#include "Config/LoggerConfigLang.h"
#include "NodeValue/ExpressionType.h"
#include "Service/Tokenizer.h"
#include "Service/SymbolTable.h"
#include "Service/StatementParser.h"
#include "Interpreter/Value/Script.h"

using SymbolTablePtr = std::unique_ptr<ska::SymbolTable>;
using ParserPtr = std::unique_ptr<ska::StatementParser>;

const auto reservedKeywords = ska::ReservedKeywordsPool{};


ska::Script ASTFromInputSemanticExpressionType(std::unordered_map<std::string, ska::ScriptHandlePtr>& scriptCache, const std::string& input, ParserPtr& parser_test) {
	static auto refCounter = 0;
    
	auto tokenizer = ska::Tokenizer { reservedKeywords, input };
	const auto tokens = tokenizer.tokenize();
	auto reader = ska::Script { scriptCache, "main", std::move(tokens) };
	parser_test = std::make_unique<ska::StatementParser> ( reservedKeywords );
    return reader;
}

TEST_CASE("[ExpressionType]") {
    ParserPtr parser_test;
	auto scriptCache = std::unordered_map<std::string, ska::ScriptHandlePtr>{};
    auto script = ASTFromInputSemanticExpressionType(scriptCache, "{var toto = 2;}", parser_test);
	script.parse(*parser_test);
	auto* symbol_test = &script.symbols();
	auto& table = *script.symbols().nested()[0];
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

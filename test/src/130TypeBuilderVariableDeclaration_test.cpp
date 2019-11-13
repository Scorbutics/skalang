#include <iostream>

#include <doctest.h>
#include "Config/LoggerConfigLang.h"
#include "Service/StatementParser.h"
#include "Service/ReservedKeywordsPool.h"
#include "Service/SymbolTable.h"
#include "Service/TypeBuilder/TypeBuilderVariableDeclaration.h"
#include "TypeBuilderTestCommon.h"
#include "Service/ASTFactory.h"
#include "NodeValue/ScriptAST.h"
#include "Service/TypeCrosser/TypeCrossExpression.h"

TEST_CASE("[TypeBuilderVariableDeclaration]") {
	auto scriptCache = ska::ScriptCacheAST {};
	DataTestContainer data;
	auto script = TypeBuilderTestCommonBuildAST(scriptCache, "", data, false);
	script.parse(*data.parser);
		auto typeBuilder = ska::TypeBuilderOperator<ska::Operator::VARIABLE_DECLARATION>{};
	
	auto nameToken = ska::Token{"toto", ska::TokenType::IDENTIFIER, {} };
	auto valueToken = ska::Token{ "1", ska::TokenType::DIGIT, {} };
	auto typeCrosser = ska::TypeCrosser{};

	auto valueNode = ska::ASTFactory::MakeLogicalNode(valueToken);
	auto result = std::array<ska::TypeBuildUnitPtr, static_cast<std::size_t>(ska::Operator::UNUSED_Last_Length)>{};
	result[static_cast<std::size_t>(ska::Operator::BINARY)] = std::make_unique<ska::TypeBuilderOperator<ska::Operator::BINARY>>(typeCrosser);
	result[static_cast<std::size_t>(ska::Operator::UNARY)] = std::make_unique<ska::TypeBuilderOperator<ska::Operator::UNARY>>();
	result[static_cast<std::size_t>(ska::Operator::LITERAL)] = std::make_unique<ska::TypeBuilderOperator<ska::Operator::LITERAL>>();

	valueNode->buildType(result, script);
	auto children = std::vector<ska::ASTNodePtr>{ };
	children.push_back(std::move(valueNode));
	auto node = ska::ASTFactory::MakeNode<ska::Operator::VARIABLE_DECLARATION>(std::move(nameToken), std::move(children));
	auto type = typeBuilder.build(script, *node);
	CHECK(type == ska::ExpressionType::INT);
}
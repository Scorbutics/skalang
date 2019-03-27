#include <iostream>

#include <doctest.h>
#include "Service/StatementParser.h"
#include "Service/ReservedKeywordsPool.h"
#include "Service/SymbolTable.h"
#include "Service/TypeBuilder/TypeBuilderVariableDeclaration.h"
#include "TypeBuilderTestCommon.h"
#include "Service/ASTFactory.h"
#include "Service/Script.h"

TEST_CASE("[TypeBuilderVariableDeclaration]") {
	auto scriptCache = std::unordered_map<std::string, ska::ScriptHandlePtr> {};
	DataTestContainer data;
	auto script = TypeBuilderTestCommonBuildAST(scriptCache, "", data, false);
	script.parse(*data.parser);
		auto typeBuilder = ska::TypeBuilderOperator<ska::Operator::VARIABLE_DECLARATION>{};
	
	auto nameToken = ska::Token{"toto", ska::TokenType::IDENTIFIER };
	auto valueToken = ska::Token{ "1", ska::TokenType::DIGIT };
	
	auto valueNode = ska::ASTFactory::MakeLogicalNode(valueToken);
	valueNode->buildType(script);
	auto children = std::vector<ska::ASTNodePtr>{ };
	children.push_back(std::move(valueNode));
	auto node = ska::ASTFactory::MakeNode<ska::Operator::VARIABLE_DECLARATION>(std::move(nameToken), std::move(children));
	auto type = typeBuilder.build(script, *node);
	CHECK(type == ska::ExpressionType::INT);
}

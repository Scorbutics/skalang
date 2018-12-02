#include <iostream>

#include <doctest.h>
#include "Parser.h"
#include "ReservedKeywordsPool.h"
#include "SymbolTable.h"
#include "TypeBuilderVariableDeclaration.h"
#include "TypeBuilderTestCommon.h"

TEST_CASE("[TypeBuilderVariableDeclaration]") {
	DataTestContainer data;
	auto ast = TypeBuilderTestCommonBuildAST("", data, false);
	auto& symbols = *data.symbols;
	auto typeBuilder = ska::TypeBuilderOperator<ska::Operator::VARIABLE_DECLARATION>{};
	
	auto nameToken = ska::Token{"toto", ska::TokenType::IDENTIFIER };
	auto valueToken = ska::Token{ "1", ska::TokenType::DIGIT };
	
	auto valueNode = ska::ASTNode::MakeLogicalNode(valueToken);
	valueNode->buildType(symbols);
	auto children = std::vector<ska::ASTNodePtr>{ };
	children.push_back(std::move(valueNode));
	auto node = ska::ASTNode::MakeNode<ska::Operator::VARIABLE_DECLARATION>(std::move(nameToken), std::move(children));
	auto type = typeBuilder.build(symbols, *node);
	CHECK(type == ska::ExpressionType::INT);
}

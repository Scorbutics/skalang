#include <iostream>

#include <doctest.h>
#include "TypeBuilderTestCommon.h"
#include "Service/TypeBuilder/TypeBuilderBinary.h"
#include "Service/ASTFactory.h"

TEST_CASE("[TypeBuilderBinary]") {
	DataTestContainer data;
	auto ast = TypeBuilderTestCommonBuildAST("", data, false);
	auto typeBuilder = ska::TypeBuilderOperator<ska::Operator::BINARY>{};
	
	auto value1Node = ska::ASTFactory::MakeLogicalNode(ska::Token{"1", ska::TokenType::DIGIT});
	auto value2Node = ska::ASTFactory::MakeLogicalNode(ska::Token{"1", ska::TokenType::STRING});
	auto node = ska::ASTFactory::MakeLogicalNode(ska::Token{"+", ska::TokenType::SYMBOL}, std::move(value1Node), std::move(value2Node));
	
	(*node)[0].buildType(*data.symbols);
	(*node)[1].buildType(*data.symbols);
	
	//This one makes a "crossType"
	auto type = typeBuilder.build(*data.symbols, *node);
	CHECK(type == ska::ExpressionType::STRING);
}

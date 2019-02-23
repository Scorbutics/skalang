#include <iostream>

#include <doctest.h>
#include "TypeBuilderTestCommon.h"
#include "Service/TypeBuilder/TypeBuilderBinary.h"
#include "Service/ASTFactory.h"
#include "Service/Script.h"

TEST_CASE("[TypeBuilderBinary]") {
	auto scriptCache = std::unordered_map<std::string, ska::ScriptHandlePtr>{};
	DataTestContainer data;
	auto script = TypeBuilderTestCommonBuildAST(scriptCache, "", data, false);
	script.parse(*data.parser);
	auto typeBuilder = ska::TypeBuilderOperator<ska::Operator::BINARY>{};
	
	auto value1Node = ska::ASTFactory::MakeLogicalNode(ska::Token{"1", ska::TokenType::DIGIT});
	auto value2Node = ska::ASTFactory::MakeLogicalNode(ska::Token{"1", ska::TokenType::STRING});
	auto node = ska::ASTFactory::MakeLogicalNode(ska::Token{"+", ska::TokenType::SYMBOL}, std::move(value1Node), std::move(value2Node));
	
	(*node)[0].buildType(script.symbols());
	(*node)[1].buildType(script.symbols());
	
	//This one makes a "crossType"
	auto type = typeBuilder.build(script.symbols(), *node);
	CHECK(type == ska::ExpressionType::STRING);
}

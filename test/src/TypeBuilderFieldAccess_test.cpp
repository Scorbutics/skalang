#include <iostream>

#include <doctest.h>
#include "TypeBuilderTestCommon.h"
#include "Service/TypeBuilder/TypeBuilderFieldAccess.h"
#include "Service/Script.h"

TEST_CASE("[TypeBuilderFieldAccess]") {
	auto scriptCache = std::unordered_map<std::string, ska::ScriptHandlePtr>{};
	DataTestContainer data;
	auto script = TypeBuilderTestCommonBuildAST(scriptCache, "var Factory = function() : var { var pdv = 0; return { pdv : pdv }; }; var obj = Factory(); obj.pdv;", data);
	auto ast = script.parse(*data.parser);
	auto& symbols = script.symbols();

	const auto& node = (*ast)[2];

	auto typeBuilder = ska::TypeBuilderOperator<ska::Operator::FIELD_ACCESS>{};
	auto type = typeBuilder.build(symbols, node);
	CHECK(type == ska::ExpressionType::INT);
}

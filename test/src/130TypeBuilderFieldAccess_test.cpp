#include <iostream>

#include <doctest.h>
#include "TypeBuilderTestCommon.h"
#include "Service/TypeBuilder/TypeBuilderFieldAccess.h"
#include "NodeValue/ScriptAST.h"

TEST_CASE("[TypeBuilderFieldAccess]") {
	DataTestContainer data;
	auto scriptCache = ska::ScriptCacheAST{};
	auto script = TypeBuilderTestCommonBuildAST(scriptCache, "Factory = function() : var do pdv = 0\n return { pdv = pdv }\n end\n obj = Factory()\n obj.pdv\n", data);
	script.parse(*data.parser);

	const auto& node = (script.rootNode())[2];

	auto typeBuilder = ska::TypeBuilderOperator<ska::Operator::FIELD_ACCESS>{};
	auto type = typeBuilder.build(script, node);
	CHECK(type == ska::ExpressionType::INT);
}

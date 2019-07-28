#include <iostream>

#include <doctest.h>
#include "TypeBuilderTestCommon.h"
#include "Service/TypeBuilder/TypeBuilderFieldAccess.h"
#include "Interpreter/Value/Script.h"

TEST_CASE("[TypeBuilderFieldAccess]") {
	DataTestContainer data;
	auto scriptCache = std::unordered_map<std::string, ska::ScriptHandlePtr>{};
	auto script = TypeBuilderTestCommonBuildAST(scriptCache, "var Factory = function() : var { var pdv = 0; return { pdv : pdv }; }; var obj = Factory(); obj.pdv;", data);
	script.parse(*data.parser);

	const auto& node = (script.rootNode())[2];

	auto typeBuilder = ska::TypeBuilderOperator<ska::Operator::FIELD_ACCESS>{};
	auto type = typeBuilder.build(script, node);
	CHECK(type == ska::ExpressionType::INT);
}

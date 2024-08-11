#include <iostream>

#include <doctest.h>
#include "TypeBuilderTestCommon.h"
#include "Service/TypeBuilder/TypeBuilderFieldAccess.h"
#include "NodeValue/ScriptAST.h"

TEST_CASE("[TypeBuilderFieldAccess]") {
	DataTestContainer data;
	auto scriptCache = ska::ScriptCacheAST{};
	auto script = TypeBuilderTestCommonBuildAST(scriptCache,
	R"script(

Factory = function() : var
	pdv1 = 0
	return {
		pdv = pdv1
	}
end

obj = Factory()
obj.pdv

)script", data);
	script.parse(*data.parser);

	const auto& node = (script.rootNode())[2];

	auto typeBuilder = ska::TypeBuilderOperator<ska::Operator::FIELD_ACCESS>{};
	auto type = typeBuilder.build(script, node).type;
	CHECK(type == ska::ExpressionType::INT);
}

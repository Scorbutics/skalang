#include <iostream>

#include <doctest.h>
#include "TypeBuilderTestCommon.h"
#include "Service/TypeBuilder/TypeBuilderBinary.h"


TEST_CASE("[TypeBuilderFieldAccess]") {
	DataTestContainer data;
	auto ast = TypeBuilderTestCommonBuildAST("var Factory = function() : var { var pdv = 0; return { pdv : pdv }; }; var obj = Factory(); obj.pdv;", data);
	auto& symbols = *data.symbols;

	const auto& node = (*ast)[2];

	auto typeBuilder = ska::TypeBuilderOperator<ska::Operator::FIELD_ACCESS>{};
	auto type = typeBuilder.build(symbols, node);
	CHECK(type == ska::ExpressionType::INT);
}

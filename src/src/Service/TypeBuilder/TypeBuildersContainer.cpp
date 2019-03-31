#include <cassert>
#include "TypeBuildUnit.h"
#include "TypeBuilderOperator.h"
#include "TypeBuildersContainer.h"

namespace ska {
	template<Operator op>
	inline void TypeBuilderMakeBuilder(TypeBuildersContainer& result) {
		result[static_cast<std::size_t>(op)] = std::make_unique<TypeBuilderOperator<op>>();
	}
}

ska::TypeBuildersContainer ska::BuildTypeBuildersContainer() {
	auto result = TypeBuildersContainer {};
	TypeBuilderMakeBuilder<Operator::BINARY>(result);
	TypeBuilderMakeBuilder<Operator::UNARY>(result);
	TypeBuilderMakeBuilder<Operator::LITERAL>(result);
	TypeBuilderMakeBuilder<Operator::VARIABLE_DECLARATION>(result);
	TypeBuilderMakeBuilder<Operator::VARIABLE_DECLARATION>(result);
	TypeBuilderMakeBuilder<Operator::VARIABLE_AFFECTATION>(result);
	TypeBuilderMakeBuilder<Operator::PARAMETER_DECLARATION>(result);
	TypeBuilderMakeBuilder<Operator::ARRAY_DECLARATION>(result);
	TypeBuilderMakeBuilder<Operator::ARRAY_USE>(result);
	TypeBuilderMakeBuilder<Operator::USER_DEFINED_OBJECT>(result);
	TypeBuilderMakeBuilder<Operator::FUNCTION_CALL>(result);
	TypeBuilderMakeBuilder<Operator::FUNCTION_PROTOTYPE_DECLARATION>(result);
	TypeBuilderMakeBuilder<Operator::FUNCTION_DECLARATION>(result);
	TypeBuilderMakeBuilder<Operator::FIELD_ACCESS>(result);
	TypeBuilderMakeBuilder<Operator::FOR_LOOP>(result);
	TypeBuilderMakeBuilder<Operator::IMPORT>(result);
	TypeBuilderMakeBuilder<Operator::EXPORT>(result);
	TypeBuilderMakeBuilder<Operator::BLOCK>(result);
	TypeBuilderMakeBuilder<Operator::IF>(result);
	TypeBuilderMakeBuilder<Operator::IF_ELSE>(result);
	TypeBuilderMakeBuilder<Operator::RETURN>(result);
	TypeBuilderMakeBuilder<Operator::SCRIPT_LINK>(result);

	for (std::size_t i = 0; i < MaxOperatorSize; i++) {
		assert(result[i] != nullptr);
	}
	return result;
}
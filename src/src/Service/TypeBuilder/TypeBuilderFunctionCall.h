#pragma once

#include "TypeBuilderOperator.h"
#include "Operation/OperationTypeFunctionCall.h"
#include "Operation/OperationTypeFunctionMemberCall.h"

namespace ska {
    SKALANG_BUILDER_TYPE_OPERATOR_DEFINE(Operator::FUNCTION_CALL);
    SKALANG_BUILDER_TYPE_OPERATOR_DEFINE(Operator::FUNCTION_MEMBER_CALL);
}

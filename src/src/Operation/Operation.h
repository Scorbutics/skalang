#pragma once

#define SKALANG_OPERATION_DEFINE(OperatorType)\
    template<>\
    struct Operation<OperatorType> {};
	
namespace ska {
	template<Operator Op>
	class Operation {};
}

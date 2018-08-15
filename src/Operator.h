#pragma once
namespace ska {
	enum class Operator {
		BINARY,
		UNARY,
		LITERAL,
		VARIABLE_DECLARATION,
		FUNCTION_CALL,
		FUNCTION_DECLARATION,
		FOR_LOOP,
		BLOCK,
		IF,
		IF_ELSE
	};
}

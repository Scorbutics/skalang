#pragma once
namespace ska {
	enum class Operator {
		BINARY,
		UNARY,
		LITERAL,
		VARIABLE_DECLARATION,
		VARIABLE_AFFECTATION,
		PARAMETER_DECLARATION,
        USER_DEFINED_OBJECT,
        FUNCTION_CALL,
		FUNCTION_DECLARATION,
		FIELD_ACCESS,
		FOR_LOOP,
		BLOCK,
		IF,
		IF_ELSE
	};

    static constexpr const char* OperatorSTR[] = {
		"BINARY",
		"UNARY",
		"LITERAL",
		"VARIABLE_DECLARATION",
		"VARIABLE_AFFECTATION",
		"PARAMETER_DECLARATION",
        "USER_DEFINED_OBJECT",
        "FUNCTION_CALL",
		"FUNCTION_DECLARATION",
		"FIELD_ACCESS",
		"FOR_LOOP",
		"BLOCK",
		"IF",
		"IF_ELSE"
	};

    static inline const char* OperatorPrint(Operator op) {
        return OperatorSTR[static_cast<std::size_t>(op)];
    }
}

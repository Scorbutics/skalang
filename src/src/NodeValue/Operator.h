#pragma once
#include <cstddef>
#include <ostream>

namespace ska {
	enum class Operator {
		BINARY,
        UNARY,
		LITERAL,
		VARIABLE_DECLARATION,
		VARIABLE_AFFECTATION,
		PARAMETER_DECLARATION,
		ARRAY_DECLARATION,
		ARRAY_USE,
        USER_DEFINED_OBJECT,
        FUNCTION_CALL,
		FUNCTION_PROTOTYPE_DECLARATION,
		FUNCTION_DECLARATION,
		FIELD_ACCESS,
		FOR_LOOP,
		IMPORT,
		EXPORT,
		BLOCK,
		IF,
		IF_ELSE,
		RETURN,
		SCRIPT_LINK,
		UNUSED_Last_Length
	};

    static constexpr const char* OperatorSTR[] = {
		"BINARY",
        "UNARY",
		"LITERAL",
		"VARIABLE_DECLARATION",
		"VARIABLE_AFFECTATION",
		"PARAMETER_DECLARATION",        
		"ARRAY_DECLARATION",
		"ARRAY_USE",
        "USER_DEFINED_OBJECT",
        "FUNCTION_CALL",
		"FUNCTION_PROTOTYPE_DECLARATION",
		"FUNCTION_DECLARATION",
		"FIELD_ACCESS",
		"FOR_LOOP",
		"IMPORT",
		"EXPORT",
		"BLOCK",
		"IF",
		"IF_ELSE",
		"RETURN",
		"SCRIPT_LINK"
	};

    static inline const char* OperatorPrint(Operator op) {
        return OperatorSTR[static_cast<std::size_t>(op)];
    }

	inline std::ostream& operator<<(std::ostream& stream, const Operator& op) {
		stream << ska::OperatorPrint(op);
		return stream;
	}
}

#pragma once

namespace ska {
	enum class ExpressionType {
		VOID,
		OBJECT,
		FUNCTION,
		INT,
		FLOAT,
		STRING,
		//??
		ARRAY
	};

	static constexpr const char* ExpressionTypeSTR[] = {
		"void",
		"object",
		"function",
		"int",
		"float",
		"string",
		"array"
	};
}

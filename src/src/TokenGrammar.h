#pragma once

namespace ska {
	enum class TokenGrammar {
		//Reserved keywords
		FOR,
		IF,
		ELSE,
		VARIABLE,
		FUNCTION,
		RETURN,
		INT,
		FLOAT,
		STRING,
		//Reserved symbols
		METHOD_CALL_OPERATOR,
		AFFECTATION,
		BLOCK_BEGIN,
		BLOCK_END,
		PARENTHESIS_BEGIN,
		PARENTHESIS_END,
		BRACKET_BEGIN,
		BRACKET_END,
		STRING_DELIMITER,
		STATEMENT_END,
		ARGUMENT_DELIMITER,
		TYPE_DELIMITER,
		//The last one for enum length
		UNUSED_Last_Length
	};


	constexpr const char* TokenGrammarSTR[] = {
		"for",
		"if",
		"else",
		"var",
		"function",
		"return",
		"int",
		"float",
		"string",
		".",
		"=",
		"{",
		"}",
		"(",
		")",
		"[",
		"]",
		"\"",
		";",
		",",
		":"
	};
}

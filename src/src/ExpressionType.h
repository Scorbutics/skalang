#pragma once
#include <unordered_map>

namespace ska {
	struct Type;
}

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
		"var",
		"function",
		"int",
		"float",
		"string",
		"array"
	};

	class ScopedSymbolTable;

    static std::unordered_map<std::string, ExpressionType> ExpressionTypeMapBuild() {
        auto result = std::unordered_map<std::string, ExpressionType>{};
        static constexpr auto ExpressionTypeSize = 7;
        for(auto index = 0u; index < ExpressionTypeSize; index++) {
            result[ExpressionTypeSTR[index]] = static_cast<ExpressionType>(index);
        }
        return result;
    }

    static const auto ExpressionTypeMap = ExpressionTypeMapBuild();

}

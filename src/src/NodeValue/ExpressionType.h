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
		ARRAY,
		BOOLEAN,
		UNUSED_Last_Length
	};

	static constexpr const char* ExpressionTypeSTR[] = {
		"void",
		"var",
		"function",
        "int",
		"float",
		"string",
		"array",
		"bool"
	};

	template<class T>
	struct ExpressionTypeFromNative;

	template<>
	struct ExpressionTypeFromNative<int> {
		static constexpr auto value = ExpressionType::INT;
	};
    
    template<>
	struct ExpressionTypeFromNative<bool> {
		static constexpr auto value = ExpressionType::BOOLEAN;
	};

	template<>
	struct ExpressionTypeFromNative<float> {
		static constexpr auto value = ExpressionType::FLOAT;
	};

	template<>
	struct ExpressionTypeFromNative<double> {
		static constexpr auto value = ExpressionType::FLOAT;
	};

	template<>
	struct ExpressionTypeFromNative<std::string> {
		static constexpr auto value = ExpressionType::STRING;
	};

	class ScopedSymbolTable;

    static std::unordered_map<std::string, ExpressionType> ExpressionTypeMapBuild() {
        auto result = std::unordered_map<std::string, ExpressionType>{};
        static constexpr auto ExpressionTypeSize = static_cast<std::size_t>(ExpressionType::UNUSED_Last_Length);
        for(auto index = 0u; index < ExpressionTypeSize; index++) {
            result[ExpressionTypeSTR[index]] = static_cast<ExpressionType>(index);
        }
        return result;
    }

    static const auto ExpressionTypeMap = ExpressionTypeMapBuild();

}


#include "Type.h"

ska::ExpressionType ska::Type::crossTypes(char op, const Type& type2) const {
	const auto& type1 = m_type;
	constexpr auto TypeMapSize = 7;

	static int typeMapOperatorPlus[TypeMapSize][TypeMapSize] = {
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 3, 4, 5, 0 },
		{ 0, 0, 0, 4, 4, 0, 0 },
		{ 0, 0, 0, 5, 0, 5, 0 },
		{ 0, 0, 0, 0, 0, 0, 6 }
	};

	static int typeMapOperatorMinus[TypeMapSize][TypeMapSize] = {
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 3, 4, 0, 0 },
		{ 0, 0, 0, 4, 4, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 }
	};

	static int typeMapOperatorMul[TypeMapSize][TypeMapSize] = {
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 3, 4, 5, 0 },
		{ 0, 0, 0, 4, 4, 0, 0 },
		{ 0, 0, 0, 5, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 }
	};

	static int typeMapOperatorDiv[TypeMapSize][TypeMapSize] = {
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 3, 4, 0, 0 },
		{ 0, 0, 0, 4, 4, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 }
	};

	static int typeMapOperatorEqual[TypeMapSize][TypeMapSize] = {
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 1, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 3, 4, 5, 0 },
		{ 0, 0, 0, 3, 4, 5, 0 },
		{ 0, 0, 0, 3, 4, 5, 0 },
		{ 0, 0, 0, 0, 0, 0, 6 }
	};

	int(*selectedTypeMap)[TypeMapSize];

	switch (op) {
	case '-':
		selectedTypeMap = typeMapOperatorMinus;
		break;
	case '+':
		selectedTypeMap = typeMapOperatorPlus;
		break;
	case '/':
		selectedTypeMap = typeMapOperatorDiv;
		break;
	case '*':
		selectedTypeMap = typeMapOperatorMul;
		break;
	case '=':
		selectedTypeMap = typeMapOperatorEqual;
		break;

	default: {
		/*std::cout << "Unknown operator \"" << op << "\", returning by default first type (of index " <<
			static_cast<std::size_t>(type1) << ") without checking type map" << std::endl;*/
		} return type1;
	}

	const auto typeIdResult = selectedTypeMap[static_cast<std::size_t>(type1)][static_cast<std::size_t>(type2.m_type)];
	if (typeIdResult == 0) {
		auto ss = std::stringstream{};
		ss << "Unable to use operator \"" << op << "\" on types " << ExpressionTypeSTR[static_cast<std::size_t>(type1)] << " and " << ExpressionTypeSTR[static_cast<std::size_t>(type2.m_type)];
		throw std::runtime_error(ss.str());
	}

	return static_cast<ExpressionType>(typeIdResult);		
}

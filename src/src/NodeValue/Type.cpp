#include "Config/LoggerConfigLang.h"
#include "Type.h"

const std::unordered_map<std::string, int(*)[ska::Type::TypeMapSize][ska::Type::TypeMapSize]>& ska::Type::GetMap(const std::string& op) {
    static int typeMapOperatorPlus[TypeMapSize][TypeMapSize] = {
		{ 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 3, 4, 5, 0, 0 },
		{ 0, 0, 0, 4, 4, 0, 0, 0 },
		{ 0, 0, 0, 5, 0, 5, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0 }
	};

	static int typeMapOperatorMinus[TypeMapSize][TypeMapSize] = {
		{ 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 3, 4, 0, 0, 0 },
		{ 0, 0, 0, 4, 4, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0 }
	};

	static int typeMapOperatorMul[TypeMapSize][TypeMapSize] = {
		{ 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 3, 4, 5, 0, 0 },
		{ 0, 0, 0, 4, 4, 0, 0, 0 },
		{ 0, 0, 0, 5, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0 }
	};

	static int typeMapOperatorDiv[TypeMapSize][TypeMapSize] = {
		{ 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 3, 4, 0, 0, 0 },
		{ 0, 0, 0, 4, 4, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0 }
	};

	static int typeMapOperatorEqual[TypeMapSize][TypeMapSize] = {
		{ 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 1, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 3, 4, 5, 0, 0 },
		{ 0, 0, 0, 3, 4, 5, 0, 0 },
		{ 0, 0, 0, 3, 4, 5, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 6, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 7 }
	};

    static int typeMapOperatorEquality[TypeMapSize][TypeMapSize] = {
		{ 7, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 7, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 7, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 7, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 7, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 7, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 7, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 7 }
	};

    static auto typeMap = std::unordered_map<std::string, int(*)[TypeMapSize][TypeMapSize]> {};
    typeMap.emplace("+", &typeMapOperatorPlus);
    typeMap.emplace("-", &typeMapOperatorMinus);
    typeMap.emplace("*", &typeMapOperatorMul);
    typeMap.emplace("/", &typeMapOperatorDiv);
    typeMap.emplace("=", &typeMapOperatorEqual);
    typeMap.emplace("==", &typeMapOperatorEquality);
    return typeMap;
}

ska::ExpressionType ska::Type::crossTypes(std::string op, const Type& type2) const {
	const auto& type1 = m_type;
    static const auto& TypeMap = GetMap(op);

	if (TypeMap.find(op) == TypeMap.end()) {
        auto ss = std::stringstream{};
        ss << "unknown operator " << op;
        throw std::runtime_error(ss.str());
	}

	const auto typeIdResult = (*TypeMap.at(op))[static_cast<std::size_t>(type1)][static_cast<std::size_t>(type2.m_type)];
	if (typeIdResult == 0) {
		auto ss = std::stringstream{};
		ss << "Unable to use operator \"" << op << "\" on types " << ExpressionTypeSTR[static_cast<std::size_t>(type1)] << " and " << ExpressionTypeSTR[static_cast<std::size_t>(type2.m_type)];
		throw std::runtime_error(ss.str());
	}

    SLOG(LogLevel::Info) << op << " has cross-type " << ExpressionTypeSTR[static_cast<std::size_t>(typeIdResult)];

	return static_cast<ExpressionType>(typeIdResult);		
}

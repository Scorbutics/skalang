#include "Config/LoggerConfigLang.h"

#include "Symbol.h"
#include "Type.h"
#include "Service/SymbolTable.h"

const std::unordered_map<std::string, int(*)[ska::Type::TypeMapSize][ska::Type::TypeMapSize]>& ska::Type::GetMap(const std::string& op) {
    static int typeMapOperatorPlus[TypeMapSize][TypeMapSize] = {
		{ 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 6, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 3, 4, 5, 6, 0 },
		{ 0, 0, 0, 4, 4, 0, 6, 0 },
		{ 0, 0, 0, 5, 0, 5, 6, 0 },
		{ 0, 6, 0, 6, 6, 6, 6, 6 },
        { 0, 0, 0, 0, 0, 0, 6, 0 }
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
		{ 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 7, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 7, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 7, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 7, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 7, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 7, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 7 }
	};

	static int typeMapOperatorLogical[TypeMapSize][TypeMapSize] = {
		{ 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 7, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 7, 7, 7, 0, 0 },
		{ 0, 0, 0, 7, 7, 7, 0, 0 },
		{ 0, 0, 0, 7, 7, 7, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 7, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 7 }
	};

	static int typeMapOperatorLogicalBool[TypeMapSize][TypeMapSize] = {
		{ 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 7 }
	};

    static auto typeMap = std::unordered_map<std::string, int(*)[TypeMapSize][TypeMapSize]> {};
    typeMap.emplace("+", &typeMapOperatorPlus);
    typeMap.emplace("-", &typeMapOperatorMinus);
    typeMap.emplace("*", &typeMapOperatorMul);
    typeMap.emplace("/", &typeMapOperatorDiv);
    typeMap.emplace("=", &typeMapOperatorEqual);
    typeMap.emplace("==", &typeMapOperatorEquality);
	typeMap.emplace("!=", &typeMapOperatorEquality);
	typeMap.emplace("&&", &typeMapOperatorLogicalBool);
	typeMap.emplace("||", &typeMapOperatorLogicalBool);
	typeMap.emplace("<", &typeMapOperatorLogical);
	typeMap.emplace("<=", &typeMapOperatorLogical);
	typeMap.emplace(">", &typeMapOperatorLogical);
	typeMap.emplace(">=", &typeMapOperatorLogical);
    return typeMap;
}

bool ska::Type::checkSameObjectTypes(const Type& type2) const {
	return *this == type2;
}

ska::Type ska::Type::crossTypes(std::string op, const Type& type2) const {
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
		ss << "Unable to use operator \"" << op << "\" on types " << (*this) << " and " << type2;
		throw std::runtime_error(ss.str());
	}

	switch (static_cast<ExpressionType>(typeIdResult)) {
	case ExpressionType::OBJECT:
		if (op == "=") {
			if (*this != type2) {
				return Type{};
			}
		}
		break;

	case ExpressionType::ARRAY: {
		auto checkArraySubTypeCoherence = (*this == type2) || (type2.size() > 0 ? type2.compound()[0] == *this : (size() > 0 && m_compound[0] == type2));
		if (!checkArraySubTypeCoherence) {
			auto ss = std::stringstream{};
			ss << "Unable to use operator \"" << op << "\" on types " << (*this) << " and " << type2;
			throw std::runtime_error(ss.str());
		}
		return type2.size() > 0 ? type2 : *this;
		} 
	}

	

    SLOG(LogLevel::Info) << op << " has cross-type " << ExpressionTypeSTR[static_cast<std::size_t>(typeIdResult)];

	return Type{ static_cast<ExpressionType>(typeIdResult) };
}

const ska::Symbol* ska::Type::operator[](const std::string& fieldName) const {
	return hasSymbol() ? (*m_symbol)[fieldName] : nullptr;
}

ska::Type::Type(const Symbol* symbol, ExpressionType t) :
	m_type(t),
	m_symbol(symbol),
	m_symbolAlias(symbol != nullptr ? symbol->getName() : "") {
}

ska::Type ska::Type::updateSymbol(const SymbolTable& symbols) const {
	auto result = Type{ symbols[m_symbolAlias], m_type };
	result.m_symbolAlias = m_symbolAlias;
	return result;
}

bool ska::Type::operator==(const Type& t) const {	
	if (!m_symbolAlias.empty() && !t.m_symbolAlias.empty() && m_symbolAlias == t.m_symbolAlias) {
		return true;
	}

	if (hasSymbol() && t.hasSymbol()) {
		return m_symbol == t.m_symbol;
	}

	return m_type == t.m_type && m_compound == t.m_compound;
}

std::ostream& ska::operator<<(std::ostream& stream, const ska::Type& type) {
	const auto mainType = ska::ExpressionTypeSTR[static_cast<std::size_t>(type.m_type)];
	auto addedSymbolPart = (type.m_symbolAlias.empty() ? "" : (" " + type.m_symbolAlias));
	if (type.m_compound.empty()) {
		stream << mainType << addedSymbolPart;
	}
	else {
		stream << mainType << addedSymbolPart << " (";
		for (const auto& childType : type.m_compound) {
			stream << " - " << childType;
		}
		stream << ")";
	}

	return stream;
}

#include "Config/LoggerConfigLang.h"

#include "Symbol.h"
#include "Type.h"
#include "Service/SymbolTable.h"
#include "Service/TypeCrosser/TypeCrossExpression.h"

ska::Type ska::Type::crossTypes(const TypeCrosser& crosser, std::string op, const Type& type2) const {
	return crosser.cross(op, *this, type2);
}

const ska::Symbol* ska::Type::operator[](const std::string& fieldName) const {
	return hasSymbol() ? (*m_symbol)[fieldName] : nullptr;
}

ska::Type::Type(const Symbol* symbol, ExpressionType t) :
	m_type(t),
	m_symbol(symbol) {
}

bool ska::Type::operator==(const Type& t) const {	
	if (hasSymbol() && t.hasSymbol()) {
		return *m_symbol == *t.m_symbol;
	}

	return equalIgnoreSymbol(t);
}

bool ska::Type::equalIgnoreSymbol(const Type& t) const {
	return m_type == t.m_type && m_compound == t.m_compound;
}

std::ostream& ska::operator<<(std::ostream& stream, const ska::Type& type) {
	const auto mainType = ExpressionTypeSTR[static_cast<std::size_t>(type.m_type)];
	auto addedSymbolPart = (type.m_symbol == nullptr ? "" : (" " + type.m_symbol->name()));
	if (type.m_compound.empty()) {
		stream << mainType << addedSymbolPart;
	} else {
		stream << mainType << addedSymbolPart << " (";
		std::size_t rindex = type.m_compound.size() - 1;
		for (const auto& childType : type.m_compound) {
			stream << childType << (rindex == 0 ? "" : " - ");
			rindex--;
		}
		stream << ")";
	}

	return stream;
}

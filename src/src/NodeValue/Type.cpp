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

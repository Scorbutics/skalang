#include <cassert>
#include "BridgeFunction.h"
#include "NodeValue/Symbol.h"

bool ska::BridgeFunction::isVoid() const { return m_function.symbol == nullptr || m_function.symbol->nativeType() ==  ExpressionType::VOID; }

std::string ska::BridgeField::name() const {
	return symbol == nullptr ? "" : symbol->name();
}

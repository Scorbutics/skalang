#include <cassert>
#include <algorithm>
#include "BridgeFunction.h"
#include "NodeValue/Symbol.h"

bool ska::BridgeFunction::isVoid() const { return m_function.symbol == nullptr || m_function.symbol->nativeType() ==  ExpressionType::VOID; }

std::string ska::BridgeField::name() const {
	return symbol == nullptr ? "" : symbol->name();
}

std::vector<ska::BridgeFunction> ska::BridgeFunction::makeFunctions() const {
    auto result = std::vector<BridgeFunction>{};
    if (m_fields.empty()) {
        return result;
    }
    result.resize(m_fields.size());
    std::transform(m_fields.begin(), m_fields.end(), result.begin(), [](const auto& field) {
        assert(field.symbol != nullptr);
        return BridgeFunction{ *field.symbol };
    });
    return result;
}

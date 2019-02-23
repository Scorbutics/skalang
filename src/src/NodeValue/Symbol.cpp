#include "NodeValue/Symbol.h"
#include "Service/ScopedSymbolTable.h"
#include "Service/Script.h"

const ska::Symbol* ska::Symbol::operator[](const std::string& fieldSymbolName) const {
	if(std::holds_alternative<ScopedSymbolTable*>(m_data)) {
		auto* scopedTable = std::get<ScopedSymbolTable*>(m_data);
		assert(scopedTable != nullptr);

		if (!scopedTable->children().empty()) {
			for(const auto& innerSymbolTable : scopedTable->children()) {
				if(!innerSymbolTable->children().empty()) {
					const auto& st = *innerSymbolTable->children().back();
					const auto* result = st[fieldSymbolName];
					if(result != nullptr) {
						return result;
					}
				}
			}
		}

		SLOG(ska::LogLevel::Debug) << "Unable to find " << fieldSymbolName << " in this symbol";
		return nullptr;
	}

	auto* script = std::get<const Script*>(m_data);
	assert(script != nullptr);
	const auto* result = script->symbols()[fieldSymbolName];
	if(result != nullptr) {
		return result;
	}

	SLOG(ska::LogLevel::Debug) << "Unable to find " << fieldSymbolName << " in this symbol";
	return nullptr;
}

ska::Symbol* ska::Symbol::operator[](const std::string& fieldSymbolName) {
	return const_cast<Symbol*>(static_cast<const Symbol*>(this)->operator[](fieldSymbolName));
}

std::size_t ska::Symbol::size() const {
    return m_category.compound().size();
}

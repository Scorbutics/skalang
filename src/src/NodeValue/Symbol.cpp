#include "NodeValue/Symbol.h"
#include "Service/ScopedSymbolTable.h"
#include "NodeValue/ScriptAST.h"

const ska::Symbol* ska::Symbol::operator[](const std::string& fieldSymbolName) const {
	if(std::holds_alternative<ScopedSymbolTable*>(m_data)) {
		auto* scopedTable = std::get<ScopedSymbolTable*>(m_data);
		assert(scopedTable != nullptr);

		if (!scopedTable->children().empty()) {
			for(const auto& innerSymbolTable : scopedTable->children()) {
				if(!innerSymbolTable->children().empty()) {
					const auto* lastInnerSymbolElement = innerSymbolTable->children().back().get();
					assert(lastInnerSymbolElement != nullptr && (std::string{"symbol \""} + fieldSymbolName + "\" doesn't exists for element ").c_str());
					const auto* result = (*lastInnerSymbolElement)[fieldSymbolName];
					if(result != nullptr) {
						return result;
					}
				}
			}
		}

		SLOG(ska::LogLevel::Info) << "Unable to find " << fieldSymbolName << " in this symbol";
		return nullptr;
	}

	assert(std::holds_alternative<const ScriptHandleAST*>(m_data));
	auto* script = std::get<const ScriptHandleAST*>(m_data);
	assert(script != nullptr);

	SLOG(ska::LogLevel::Debug) << "Looking for " << fieldSymbolName << " in the targetted script";

	const auto* result = (script->symbols())[fieldSymbolName];
	if(result != nullptr) {
		return result;
	}

	SLOG(ska::LogLevel::Info) << "Unable to find " << fieldSymbolName << " in this symbol";
	return nullptr;
}

ska::Symbol* ska::Symbol::operator[](const std::string& fieldSymbolName) {
	return const_cast<Symbol*>(static_cast<const Symbol*>(this)->operator[](fieldSymbolName));
}

std::size_t ska::Symbol::size() const {
    return m_category.compound().size();
}

bool ska::Symbol::operator==(const Symbol& sym) const {
	
	const auto compareData = (std::holds_alternative<ScopedSymbolTable*>(m_data) ?
		std::holds_alternative<ScopedSymbolTable*>(sym.m_data) && std::get<ScopedSymbolTable*>(sym.m_data) == std::get<ScopedSymbolTable*>(m_data) :
		std::holds_alternative<const ScriptHandleAST*>(sym.m_data) && std::get<const ScriptHandleAST*>(sym.m_data) == std::get<const ScriptHandleAST*>(m_data));

	return m_name == sym.m_name && /*
		m_category== sym.m_category */
		
	compareData
		
		;
	
}

void ska::Symbol::forceType(Type t) {
	m_category = t;
	//m_category.m_symbol = this;
}

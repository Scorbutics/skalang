#include "Config/LoggerConfigLang.h"
#include "NodeValue/Symbol.h"
#include "NodeValue/ScriptAST.h"
#include "ScopedSymbolTable.h"
#include "SymbolFieldResolver.h"

SKA_LOGC_CONFIG(ska::LogLevel::Info, ska::SymbolFieldResolver)

ska::SymbolFieldResolver::SymbolFieldResolver(Variant value) :
  m_data(std::move(value)) {

  if(std::holds_alternative<ScopedSymbolTable*>(value)) {
    SLOG(ska::LogLevel::Debug) << "Creating symbol fields resolver from table";
  } else {
    SLOG(ska::LogLevel::Debug) << "Creating symbol fields resolver from script";
  }
}

const ska::Symbol* ska::SymbolFieldResolver::lookup(std::size_t tableIndex, const std::string& fieldName) const {
	if(std::holds_alternative<ScopedSymbolTable*>(m_data)) {
		auto* containingTable = std::get<ScopedSymbolTable*>(m_data);
		assert(containingTable != nullptr);

		if (containingTable->children().size() > tableIndex) {
			const auto& innerSymbolTable = containingTable->children()[tableIndex];
			if (!innerSymbolTable->children().empty()) {
				const auto* lastInnerSymbolElement = innerSymbolTable->children().back().get();
				assert(lastInnerSymbolElement != nullptr && (std::string{ "symbol \"" } +fieldName + "\" doesn't exists for element ").c_str());
				SLOG(ska::LogLevel::Info) << "Looking for symbol field \"" << fieldName << "\" in " << tableIndex << "nth child of current symbol";
				const auto* result = (*lastInnerSymbolElement)[fieldName];
				if (result != nullptr) {
					return result;
				}
			}
		}

		SLOG(ska::LogLevel::Info) << "Unable to find \"" << fieldName << "\" in this symbol in the table";
		return nullptr;
	}

	assert(std::holds_alternative<const ScriptHandleAST*>(m_data));
	auto* script = std::get<const ScriptHandleAST*>(m_data);
	assert(script != nullptr);

	SLOG(ska::LogLevel::Debug) << "Looking for " << fieldName << " in the targetted script";

	const auto* result = (script->symbols())[fieldName];
	if(result != nullptr) {
		return result;
	}

	SLOG(ska::LogLevel::Info) << "Unable to find \"" << fieldName << "\" in this symbol in script \"" << script->name() << "\"";
	return nullptr;
}

bool ska::operator==(const SymbolFieldResolver& lhs, const SymbolFieldResolver& rhs) {
  	return (std::holds_alternative<ScopedSymbolTable*>(lhs.m_data) ?
		std::holds_alternative<ScopedSymbolTable*>(rhs.m_data) && std::get<ScopedSymbolTable*>(rhs.m_data) == std::get<ScopedSymbolTable*>(lhs.m_data) :
		std::holds_alternative<const ScriptHandleAST*>(rhs.m_data) && std::get<const ScriptHandleAST*>(rhs.m_data) == std::get<const ScriptHandleAST*>(lhs.m_data));
}

bool ska::operator!=(const SymbolFieldResolver& lhs, const SymbolFieldResolver& rhs) {
  return !operator==(lhs, rhs);
}

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

template<class ThisType, class Return>
static Return Lookup(ThisType& variant, std::size_t tableIndex, const std::string& fieldName) {
		if(std::holds_alternative<ska::ScopedSymbolTable*>(variant)) {
		auto* containingTable = std::get<ska::ScopedSymbolTable*>(variant);
		assert(containingTable != nullptr);

		if (containingTable->children().size() > tableIndex) {
			auto& innerSymbolTable = containingTable->children()[tableIndex];
			if (!innerSymbolTable->children().empty()) {
				auto* lastInnerSymbolElement = innerSymbolTable->children().back().get();
				assert(lastInnerSymbolElement != nullptr && (std::string{ "symbol \"" } +fieldName + "\" doesn't exists for element ").c_str());
				SLOG_STATIC(ska::LogLevel::Info, ska::SymbolFieldResolver) << "Looking for symbol field \"" << fieldName << "\" in " << tableIndex << "nth child of current symbol";
				auto* result = (*lastInnerSymbolElement)[fieldName];
				if (result != nullptr) {
					return result;
				}
			}
		}

		SLOG_STATIC(ska::LogLevel::Info, ska::SymbolFieldResolver) << "Unable to find \"" << fieldName << "\" in this symbol in the table";
		return nullptr;
	}

	assert(std::holds_alternative<ska::ScriptHandleAST*>(variant));
	auto* script = std::get<ska::ScriptHandleAST*>(variant);
	assert(script != nullptr);

	SLOG_STATIC(ska::LogLevel::Debug, ska::SymbolFieldResolver) << "Looking for " << fieldName << " in the targetted script";

	auto* result = (script->symbols())[fieldName];
	if(result != nullptr) {
		return result;
	}

	SLOG_STATIC(ska::LogLevel::Info, ska::SymbolFieldResolver) << "Unable to find \"" << fieldName << "\" in this symbol in script \"" << script->name() << "\"";
	return nullptr;
}

const ska::Symbol* ska::SymbolFieldResolver::lookup(std::size_t tableIndex, const std::string& fieldName) const {
	return Lookup<const Variant&, const Symbol*>(m_data, tableIndex, fieldName);
}

ska::Symbol* ska::SymbolFieldResolver::lookup(std::size_t tableIndex, const std::string& fieldName) {
	return Lookup<Variant&, Symbol*>(m_data, tableIndex, fieldName);
}

bool ska::operator==(const SymbolFieldResolver& lhs, const SymbolFieldResolver& rhs) {
  	return (std::holds_alternative<ScopedSymbolTable*>(lhs.m_data) ?
		std::holds_alternative<ScopedSymbolTable*>(rhs.m_data) && std::get<ScopedSymbolTable*>(rhs.m_data) == std::get<ScopedSymbolTable*>(lhs.m_data) :
		std::holds_alternative<ScriptHandleAST*>(rhs.m_data) && std::get<ScriptHandleAST*>(rhs.m_data) == std::get<ScriptHandleAST*>(lhs.m_data));
}

bool ska::operator!=(const SymbolFieldResolver& lhs, const SymbolFieldResolver& rhs) {
  return !operator==(lhs, rhs);
}

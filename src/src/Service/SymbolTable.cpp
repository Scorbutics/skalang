#include <Utils/Observable.h>
#include "Config/LoggerConfigLang.h"

#include "SymbolTable.h"
#include "Service/StatementParser.h"
#include "Service/ASTFactory.h"
#include "Operation/Type/OperationTypeBridge.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::SymbolTable)

namespace ska::detail::symbol {
	template <bool remove, class Event>
	void manageObs(ska::StatementParser& parser, ska::SymbolTable& symbol) {
		if constexpr (remove) {
			parser.ska::Observable<Event>::removeObserver(symbol);
		} else {
			parser.ska::Observable<Event>::addObserver(symbol);
		}
	}
	
	template <class ... Events>
	inline void addAsObserver(ska::StatementParser& parser, ska::SymbolTable& symbol) {
		int _[] = { 0, (manageObs<false, Events>(parser, symbol), 0)... };
		static_cast<void>(_);
	}
	

	template <class ... Events>
	inline void removeAsObserver(ska::StatementParser& parser, ska::SymbolTable& symbol) {
		int _[] = { 0, (manageObs<true, Events>(parser, symbol), 0)... };
		static_cast<void>(_);
	}
}

ska::SymbolTable::SymbolTable(StatementParser& parser) : 
	SymbolTable() {
	m_rootTable = std::make_unique<ScopedSymbolTable>();
	m_currentTable = m_rootTable.get();
	listenParser(parser);
}

ska::SymbolTable::SymbolTable() :
	Observer<VarTokenEvent>(std::bind(&ska::SymbolTable::match, this, std::placeholders::_1)),
	Observer<BlockTokenEvent>(std::bind(&ska::SymbolTable::nestedTable, this, std::placeholders::_1)),
	Observer<FunctionTokenEvent>(std::bind(&ska::SymbolTable::matchFunction, this, std::placeholders::_1)),
    Observer<ReturnTokenEvent>(std::bind(&ska::SymbolTable::matchReturn, this, std::placeholders::_1)),
	Observer<ImportTokenEvent>(std::bind(&ska::SymbolTable::matchImport, this, std::placeholders::_1)),
	Observer<BridgeTokenEvent>(std::bind(&ska::SymbolTable::matchBridge, this, std::placeholders::_1)) {
	m_rootTable = std::make_unique<ScopedSymbolTable>();
	m_currentTable = m_rootTable.get();
}

ska::SymbolTable::~SymbolTable() {
	unlistenParser();
}

void ska::SymbolTable::listenParser(StatementParser& parser) {
	unlistenParser();
	m_parser = &parser;
	detail::symbol::addAsObserver<
		VarTokenEvent, 
		BlockTokenEvent, 
		FunctionTokenEvent, 
		ReturnTokenEvent, 
		ImportTokenEvent, 
		BridgeTokenEvent
	> (*m_parser, *this);
}

void ska::SymbolTable::unlistenParser() {
	if(m_parser != nullptr) {
		detail::symbol::removeAsObserver<
			VarTokenEvent, 
			BlockTokenEvent, 
			FunctionTokenEvent, 
			ReturnTokenEvent, 
			ImportTokenEvent, 
			BridgeTokenEvent
		> (*m_parser, *this);
		m_parser = nullptr;
	}
}

bool ska::SymbolTable::nestedTable(const BlockTokenEvent& event) {
	assert(m_currentTable != nullptr);

	switch(event.type()) {
		case BlockTokenEventType::START:
			SLOG(ska::LogLevel::Debug) << "\tNew block : adding a nested symbol table";
			m_currentTable = &m_currentTable->createNested();
			break;
		
		case BlockTokenEventType::END:
			SLOG(ska::LogLevel::Debug) << "\tBlock end : going up in nested symbol table hierarchy";
			assert(m_currentTable != nullptr);
			m_currentTable = &m_currentTable->parent();
			break;

		default:
			break;
	}
	return true;
}

bool ska::SymbolTable::matchReturn(const ReturnTokenEvent& token) {
	assert(m_currentTable != nullptr);

    switch(token.type()) {
	    case ReturnTokenEventType::START: {
			SLOG(ska::LogLevel::Debug) << "\t\tNew Return : adding a nested symbol table";
            const auto actualNameSymbol = m_currentTable->owner();
            if (actualNameSymbol == nullptr) {
                throw std::runtime_error("bad user-defined return placing : custom return must be set in a named function-constructor");
            }
            m_currentTable = &m_currentTable->createNested();
            SLOG(ska::LogLevel::Info) << "\tReturn : nested named symbol table with name : " << actualNameSymbol->getName();
        }
      break;

        default:
			SLOG(ska::LogLevel::Debug) << "\t\tReturn end: going up in nested symbol table hierarchy";
            m_currentTable = &m_currentTable->parent();
        break;
    }
	return true;
}

bool ska::SymbolTable::matchFunction(const FunctionTokenEvent& token) {
	assert(m_currentTable != nullptr);

	switch(token.type()) {

		case FunctionTokenEventType::DECLARATION_NAME: {
			auto& symbol = m_currentTable->emplace(token.name());
			SLOG(ska::LogLevel::Info) << "\t\tNew function : adding a nested symbol table named \"" << token.name() << "\"";
            m_currentTable = &m_currentTable->createNested();
			m_currentTable->link(symbol);
        } break;

        case FunctionTokenEventType::DECLARATION_STATEMENT:
			SLOG(ska::LogLevel::Debug) << "\t\tFunction end: going up in nested symbol table hierarchy";
            m_currentTable = &m_currentTable->parent();
        break;

		default:
		break;
	}
	
	return true;
}

bool ska::SymbolTable::match(const VarTokenEvent& token) {
	assert(m_currentTable != nullptr);
	
	switch(token.type()) {
		case VarTokenEventType::VARIABLE_DECLARATION:
		case VarTokenEventType::PARAMETER_DECLARATION: {
			const auto variableName = token.name();
			SLOG(ska::LogLevel::Info) << "Matching new variable : " << variableName;
			auto symbolInCurrentTable = (*m_currentTable)(variableName);
			if (symbolInCurrentTable == nullptr) {
				m_currentTable->emplace(variableName);
			} else if(
				symbolInCurrentTable->getType() != ExpressionType::FUNCTION && 
				symbolInCurrentTable->getType() != ExpressionType::VOID) {
				throw std::runtime_error("Symbol already defined : " + variableName);
			}
        } break;

		case VarTokenEventType::USE: {
			const auto variableName = token.name();
			SLOG(ska::LogLevel::Info) << "Using variable : " << variableName;
			const auto symbol = (*this)[variableName];
			if(symbol == nullptr) {
				throw std::runtime_error("Symbol not found : " + variableName);
			}       
		}
		break;
			case VarTokenEventType::AFFECTATION:
			case VarTokenEventType::FUNCTION_DECLARATION:
			break;
		default:
			throw std::runtime_error("Unmanaged variable event");
			break;
	}
	return true;
}

bool ska::SymbolTable::matchImport(const ImportTokenEvent& token) {
	assert(token.rootNode().size() == 5);
	auto& hiddenFields = token.rootNode()[3];
	for (auto& hiddenField : hiddenFields) {
		erase(hiddenField->name());
	}
	return true;
}

bool ska::SymbolTable::matchBridge(const BridgeTokenEvent& token) {
	/*const auto& scriptName = token.rootNode()[0].name();
	const auto symbol = (*this)[scriptName];
	if (symbol == nullptr) {
		return true;
	}*/
	return true;
}

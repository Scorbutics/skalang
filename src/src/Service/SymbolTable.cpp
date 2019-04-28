#include <Utils/Observable.h>
#include "Config/LoggerConfigLang.h"

#include "SymbolTable.h"
#include "Service/StatementParser.h"
#include "Service/ASTFactory.h"
#include "Operation/Type/OperationTypeScriptLink.h"
#include "Event/ScriptLinkTokenEvent.h"
#include "Interpreter/Value/Script.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::SymbolTable)

namespace ska::detail::symbol {
	template <bool remove, class Event>
	void manageObs(ska::StatementParser& parser, ska::SymbolTable& symbol) {
		if constexpr (remove) {
			parser.ska::observable_priority_queue<Event>::removeObserver(symbol);
		} else {
			parser.ska::observable_priority_queue<Event>::addObserver(symbol);
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
	internalListenParser(parser);
}

ska::SymbolTable::SymbolTable() :
	PriorityObserver<VarTokenEvent>(5, std::bind(&ska::SymbolTable::match, this, std::placeholders::_1)),
	PriorityObserver<BlockTokenEvent>(5, std::bind(&ska::SymbolTable::nestedTable, this, std::placeholders::_1)),
	PriorityObserver<FunctionTokenEvent>(5, std::bind(&ska::SymbolTable::matchFunction, this, std::placeholders::_1)),
    PriorityObserver<ReturnTokenEvent>(5, std::bind(&ska::SymbolTable::matchReturn, this, std::placeholders::_1)),
	PriorityObserver<ImportTokenEvent>(5, std::bind(&ska::SymbolTable::matchImport, this, std::placeholders::_1)),
	PriorityObserver<ScriptLinkTokenEvent>(5, std::bind(&ska::SymbolTable::matchScriptLink, this, std::placeholders::_1)) {
	m_rootTable = std::make_unique<ScopedSymbolTable>();
	m_currentTable = m_rootTable.get();
}

ska::SymbolTable::~SymbolTable() {
	internalUnlistenParser();
}

ska::ParserListenerLock ska::SymbolTable::listenParser(StatementParser& parser) {
	return ParserListenerLock{*this, parser};
}

void ska::SymbolTable::internalListenParser(StatementParser& parser) {
	internalUnlistenParser();
	m_parser = &parser;
	detail::symbol::addAsObserver<
		VarTokenEvent, 
		BlockTokenEvent, 
		FunctionTokenEvent, 
		ReturnTokenEvent, 
		ImportTokenEvent, 
		ScriptLinkTokenEvent
	> (*m_parser, *this);
}

void ska::SymbolTable::internalUnlistenParser() {
	if(m_parser != nullptr) {
		detail::symbol::removeAsObserver<
			VarTokenEvent, 
			BlockTokenEvent, 
			FunctionTokenEvent, 
			ReturnTokenEvent, 
			ImportTokenEvent, 
			ScriptLinkTokenEvent
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
            const auto actualNameSymbol = m_currentTable->directOwner();
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
            m_currentTable = &m_currentTable->createNested(&symbol);
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
	assert(token.rootNode().size() == 1);
	m_currentTable->emplace(token.rootNode()[0].name(), token.bound());
	return true;
}

bool ska::SymbolTable::matchScriptLink(const ScriptLinkTokenEvent& token) {
	if (token.bound() != nullptr && token.bound()->isBridged()) {
		//Binds the current memory table script to the bridge-script (only if it's a bridged script)
		m_currentTable->emplace(token.name(), *token.bound());
	}	
	return true;
}

ska::ParserListenerLock::ParserListenerLock(ParserListenerLock&& pll) noexcept :
	m_symbolTable(pll.m_symbolTable), 
	m_freed(pll.m_freed) {
	pll.m_freed = true;
}

ska::ParserListenerLock::ParserListenerLock(SymbolTable& symbolTable, StatementParser& parser) :
	m_symbolTable(symbolTable) {
	m_symbolTable.internalListenParser(parser);
}

void ska::ParserListenerLock::release() {
	if(!m_freed) {
		m_symbolTable.internalUnlistenParser();
		m_freed = true;
	}
}

ska::ParserListenerLock::~ParserListenerLock() {
	release();
}

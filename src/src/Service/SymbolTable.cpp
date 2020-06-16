#include "Config/LoggerConfigLang.h"
#include <Base/Patterns/Observable.h>

#include "SymbolTable.h"
#include "Service/StatementParser.h"
#include "Service/ASTFactory.h"
#include "Operation/OperationTypeScriptLink.h"
#include "Event/ScriptLinkTokenEvent.h"
#include "NodeValue/ScriptAST.h"

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
	PriorityObserver<FilterTokenEvent>(5, std::bind(&ska::SymbolTable::matchFilter, this, std::placeholders::_1)),
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
		FilterTokenEvent,
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
			FilterTokenEvent,
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
        const auto* actualNameSymbol = m_currentTable->directOwner();
        if (actualNameSymbol == nullptr) {
            throw std::runtime_error("bad user-defined return placing : custom return must be set in a named function-constructor");
        }
        m_currentTable = &m_currentTable->createNested(nullptr, true);
        SLOG(ska::LogLevel::Info) << "\tReturn : nested named symbol table with name : " << actualNameSymbol->name();
    }
  	break;

    default:
		SLOG(ska::LogLevel::Debug) << "\t\tReturn end: going up in nested symbol table hierarchy";
		m_currentTable = &m_currentTable->parent();
		auto* actualNameSymbol = m_currentTable->directOwner();
		if (actualNameSymbol == nullptr) {
			throw std::runtime_error("bad user-defined return placing : custom return must be set in a named function-constructor");
		}
		actualNameSymbol->closeTable();
    break;
	}
	return true;
}

bool ska::SymbolTable::matchFilter(const FilterTokenEvent& event) {
	assert(m_currentTable != nullptr);
	switch (event.type()) {
	case FilterTokenEventType::DECLARATION: {
		SLOG(ska::LogLevel::Info) << "\t\tNew filter-foreach-function : adding a nested unnamed symbol table";
		m_currentTable = &m_currentTable->createNested();
		m_currentTable->emplace(event.elementIterator().name());
		if (!event.indexIterator().logicalEmpty()) {
			m_currentTable->emplace(event.indexIterator().name());
		}
	} break;

	case FilterTokenEventType::DEFINITION: {
		SLOG(ska::LogLevel::Debug) << "\t\tFilter end";
		m_currentTable = &m_currentTable->parent();
	} break;

	default:
		break;
	}

	return true;

}

bool ska::SymbolTable::matchFunction(FunctionTokenEvent& token) {
	assert(m_currentTable != nullptr);

	switch(token.type()) {
	case FunctionTokenEventType::DECLARATION_NAME: {
		auto& symbol = m_currentTable->emplace(token.name());
		SLOG(ska::LogLevel::Info) << "\t\tNew function : adding a nested symbol table named \"" << token.name() << "\"";
		m_currentTable = &m_currentTable->createNested(&symbol);
		symbol.openTable();
		token.rootNode().linkSymbol(symbol);
    } break;

	case FunctionTokenEventType::FACTORY_DECLARATION_STATEMENT:
    case FunctionTokenEventType::DECLARATION_STATEMENT: {
		SLOG(ska::LogLevel::Debug) << "\t\tFunction end: going up in nested symbol table hierarchy";
		m_currentTable = &m_currentTable->parent();
		auto * symbol = (*m_currentTable)[token.name()];
		assert(symbol != nullptr);
		symbol->closeTable();
		token.rootNode().linkSymbol(*symbol);
	} break;

	default:
	break;
	}
	
	return true;
}

bool ska::SymbolTable::changeTypeIfRequired(const std::string& symbolName, const Type& value) {
	return m_currentTable->changeTypeIfRequired(symbolName, value);
}

template <class ThisType, class Return>
static Return Lookup(ThisType& currentTable, ska::SymbolTableLookup strategy, ska::SymbolTableNested depth) {
	if (!depth.childName.empty()) {
		auto* symbol = currentTable[depth.childName];
		if (symbol == nullptr) { return nullptr; }
		return (*symbol)(std::move(strategy.symbolName));
	}

	auto* selectedTable = &currentTable;
	while (depth.depth < 0) {
		selectedTable = &selectedTable->parent();
		depth.depth++;
	}

	while (depth.depth > 0 && selectedTable != nullptr && selectedTable->scopes() > 0) {
		auto currentDepth = depth.childIndex;
		if (currentDepth >= selectedTable->scopes()) {
			currentDepth = selectedTable->scopes() - 1;
		}
		selectedTable = selectedTable->child(currentDepth);
		depth.depth--;
	}

	assert(selectedTable != nullptr);

	switch (strategy.type) {
	case ska::SymbolTableLookupType::Hierarchical:
		return (*selectedTable)[std::move(strategy.symbolName)];
	case ska::SymbolTableLookupType::Direct:
		return (*selectedTable)(std::move(strategy.symbolName));
	default:
		throw std::runtime_error("unhandled enum value");
		return nullptr;
	}
}

const ska::Symbol* ska::SymbolTable::lookup(SymbolTableLookup strategy, SymbolTableNested depth) const {
	return Lookup<const ScopedSymbolTable&, const Symbol*>(*m_currentTable, std::move(strategy), std::move(depth));
}

ska::Symbol* ska::SymbolTable::lookup(SymbolTableLookup strategy, SymbolTableNested depth) {
	return Lookup<ScopedSymbolTable&, Symbol*>(*m_currentTable, std::move(strategy), std::move(depth));
}

bool ska::SymbolTable::match(VarTokenEvent& token) {
	assert(m_currentTable != nullptr);
	
	switch(token.type()) {
		case VarTokenEventType::VARIABLE_AFFECTATION:
		case VarTokenEventType::PARAMETER_DECLARATION: {
			const auto variableName = token.name();
			SLOG(ska::LogLevel::Info) << "Matching variable : " << variableName;
			auto symbolInCurrentTable = m_currentTable->exported() ? (*m_currentTable)(variableName) : (*m_currentTable)[variableName];
			if (symbolInCurrentTable == nullptr) {
				SLOG(ska::LogLevel::Info) << " that was a new variable";
				auto& symbol = m_currentTable->emplace(variableName);
				token.rootNode().linkSymbol(symbol);
			} else {
				SLOG(ska::LogLevel::Info) << " that was an existing variable";
				token.rootNode().linkSymbol(*symbolInCurrentTable);
			}
    	} break;

		case VarTokenEventType::USE: {
			const auto variableName = token.name();
			SLOG(ska::LogLevel::Info) << "Using variable : " << variableName;
			const auto symbol = (*this)[variableName];
			if(symbol == nullptr) {
				throw std::runtime_error("Symbol not found : " + variableName);
			}
			token.rootNode().linkSymbol(*symbol);
		}
		break;

		case VarTokenEventType::FUNCTION_DECLARATION: {
			auto* symbol = (*m_currentTable)[token.name()];
			assert(symbol != nullptr);
			token.rootNode().linkSymbol(*symbol);
		} break;
		case VarTokenEventType::AFFECTATION:
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

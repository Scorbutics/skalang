#include "LoggerConfigLang.h"
#include "SymbolTable.h"
#include "Parser.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::SymbolTable)
SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::ScopedSymbolTable)

ska::SymbolTable::SymbolTable(Parser& parser) :
	SubObserver<VarTokenEvent>(std::bind(&ska::SymbolTable::match, this, std::placeholders::_1), parser),
	SubObserver<BlockTokenEvent>(std::bind(&ska::SymbolTable::nestedTable, this, std::placeholders::_1), parser),
	SubObserver<FunctionTokenEvent>(std::bind(&ska::SymbolTable::matchFunction, this, std::placeholders::_1), parser),
    SubObserver<ReturnTokenEvent>(std::bind(&ska::SymbolTable::matchReturn, this, std::placeholders::_1), parser) {
		m_rootTable = std::make_unique<ScopedSymbolTable>();
		m_currentTable = m_rootTable.get();
}

bool ska::SymbolTable::nestedTable(const BlockTokenEvent& event) {
	//TODO gsl::not_null<...> pour m_currentTable
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
	//TODO gsl::not_null<...> pour m_currentTable
	assert(m_currentTable != nullptr);

    switch(token.type()) {
	    case ReturnTokenEventType::START: {
			SLOG(ska::LogLevel::Debug) << "\t\tNew Return : adding a nested symbol table";
            const auto actualNameSymbol = m_currentTable->parentSymbol();
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
	//TODO gsl::not_null<...> pour m_currentTable
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

		case FunctionTokenEventType::DECLARATION_PROTOTYPE:
		default:
		break;
	}
	
	return true;
}

bool ska::SymbolTable::match(const VarTokenEvent& token) {
	//TODO gsl::not_null<...> pour m_currentTable
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

		default:
		case VarTokenEventType::AFFECTATION:
		case VarTokenEventType::USE: {
			const auto variableName = token.name();
			SLOG(ska::LogLevel::Info) << "Using variable : " << variableName;
			const auto symbol = (*this)[variableName];
			if(symbol == nullptr) {
				throw std::runtime_error("Symbol not found : " + variableName);
			}       
		}
		break;
	}
	return true;
}




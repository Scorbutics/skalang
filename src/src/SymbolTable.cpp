#include "LoggerConfigLang.h"
#include "SymbolTable.h"
#include "Parser.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::SymbolTable)
SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::ScopedSymbolTable)

const ska::Symbol* ska::Symbol::operator[](const std::string& symbol) const {
	assert(m_scopedTable != nullptr);
	if (!m_scopedTable->children().empty()) {
		const auto& st = *m_scopedTable->children()[0];
		return st[symbol];
	}
	return nullptr;
}

ska::Symbol* ska::Symbol::operator[](const std::string& symbol) {
	return const_cast<Symbol*>(static_cast<const Symbol*>(this)->operator[](symbol));
}

std::size_t ska::Symbol::size() const {
    return m_category.compound().size();
}

ska::SymbolTable::SymbolTable(Parser& parser) :
	SubObserver<VarTokenEvent>(std::bind(&ska::SymbolTable::match, this, std::placeholders::_1), parser),
	SubObserver<BlockTokenEvent>(std::bind(&ska::SymbolTable::nestedTable, this, std::placeholders::_1), parser),
	SubObserver<FunctionTokenEvent>(std::bind(&ska::SymbolTable::matchFunction, this, std::placeholders::_1), parser),
    SubObserver<ReturnTokenEvent>(std::bind(&ska::SymbolTable::matchReturn, this, std::placeholders::_1), parser) {
		m_rootTable = std::make_unique<ScopedSymbolTable>();
		m_currentTable = m_rootTable.get();
}

bool ska::SymbolTable::nestedTable(const BlockTokenEvent& event) {
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

    switch(token.type()) {
	    case ReturnTokenEventType::START: {
            const auto actualNameSymbol = m_currentTable->parentSymbol();
            if (actualNameSymbol == nullptr) {
                throw std::runtime_error("bad user-defined return placing : custom return must be set in a named function-constructor");
            }
            m_currentTable = &m_currentTable->createNested();
            SLOG(ska::LogLevel::Info) << "\tReturn : nested named symbol table with name : " << actualNameSymbol->getName();
        }
      break;

        default:
            for (auto& field :token.rootNode()) {
                //auto& fieldValue = (*field)[0];
                m_currentTable->emplace(field->name(), ska::Type{});//fieldValue.type().value());
                //SLOG(ska::LogLevel::Info) << "\t\tfield " << (*field) << " with type " << field->type().value();
            }
            m_currentTable = &m_currentTable->parent();
        break;
    }
	return true;
}

bool ska::SymbolTable::matchFunction(const FunctionTokenEvent& token) {
	//TODO gsl::not_null<...> pour m_currentTable
	assert(m_currentTable != nullptr);
	switch(token.type()) {

		case FunctionTokenEventType::DECLARATION_PARAMETERS: {
			auto& symbol = m_currentTable->emplace(token.name());
			SLOG(ska::LogLevel::Info) << "\t\tNew function : adding a nested symbol table";
			
            auto* functionSymbolTable = &m_currentTable->createNested();
            m_currentTable = functionSymbolTable;
			
            functionSymbolTable->link(symbol);
            
			const auto parameterNumber = token.rootNode().size();
			SLOG(ska::LogLevel::Info) << "\t\tthis function (" << token.name() << ") has " << parameterNumber << " parameters : ";
			for(auto& param : token) {
				assert(!param->type().has_value());
				SLOG(ska::LogLevel::Debug) << "\t\t" << param->name();
				m_currentTable->emplace(param->name());
            }
			
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
	assert(token.rootNode().size() >= 1);
	assert(m_currentTable != nullptr);
	
	switch(token.type()) {
		case VarTokenEventType::DECLARATION: {			
			assert(token.rootNode().size() > 0);
			const auto type = token.rootNode()[0].type();
			const auto variableName = token.rootNode().name();
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
            assert(token.rootNode().size() > 0);
			const auto variableName = token.rootNode()[0].name();
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


ska::ScopedSymbolTable& ska::ScopedSymbolTable::parent() {
	return m_parent;
}


ska::Symbol& ska::ScopedSymbolTable::emplace(std::string name, Type type) {
    
    {
        auto symbol = Symbol { name, *this, type };
		SLOG(ska::LogLevel::Debug) << "\tSymbol \"" << name << "\" \"" <<  symbol.getType() << "\"";
        if(m_symbols.find(name) == m_symbols.end()) {
            m_symbols.emplace(name, std::move(symbol));
        } else {
            m_symbols.at(name) = std::move(symbol);
        }
    }

    auto& s = m_symbols.at(name);
	SLOG(ska::LogLevel::Debug) << "\tSymbol Inserted \"" << name << "\" \"" << s.getType() << "\"";
    return s;
}

ska::ScopedSymbolTable& ska::ScopedSymbolTable::createNested() {
	m_children.push_back(std::make_unique<ska::ScopedSymbolTable>(*this));
	auto& lastChild = *m_children.back();
	//No bad memory access possible when unique_ptr are moved, that's why it's safe to return the address of contained item
	//even if we move the vector or if the vector moves its content automatically 
	return lastChild;
}


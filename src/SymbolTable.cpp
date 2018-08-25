#include "SymbolTable.h"

//#define SKALANG_LOG_SYMBOL_TABLE


const ska::Symbol* ska::Symbol::operator[](const std::string& symbol) const {
    assert(m_scopedTable != nullptr);
    return (*m_scopedTable)[symbol];
}

ska::Symbol* ska::Symbol::operator[](const std::string& symbol) {
    assert(m_scopedTable != nullptr);
    return (*m_scopedTable)[symbol];
}

std::size_t ska::Symbol::size() const {
    return m_subTypes.size();
}

void ska::Symbol::link(std::vector<Symbol> subtypes, ScopedSymbolTable& table) {
    m_scopedTable = &table;
    m_scopedTable->link(*this);
    m_subTypes = std::move(subtypes);
}

ska::SymbolTable::SymbolTable(Parser& parser) :
	SubObserver<VarTokenEvent>(std::bind(&ska::SymbolTable::match, this, std::placeholders::_1), parser),
	SubObserver<BlockTokenEvent>(std::bind(&ska::SymbolTable::nestedTable, this, std::placeholders::_1), parser),
	SubObserver<FunctionTokenEvent>(std::bind(&ska::SymbolTable::matchFunction, this, std::placeholders::_1), parser),
    SubObserver<ReturnTokenEvent>(std::bind(&ska::SymbolTable::matchReturn, this, std::placeholders::_1), parser) {
		m_rootTable = std::make_unique<ScopedSymbolTable>();
		m_currentTable = m_rootTable.get();
}

bool ska::SymbolTable::nestedTable(BlockTokenEvent& event) {
	switch(event.type) {
		case BlockTokenEventType::START:
#ifdef SKALANG_LOG_SYMBOL_TABLE
			std::cout << "\tNew block : adding a nested symbol table" << std::endl;
#endif
			m_currentTable = &m_currentTable->createNested();
			break;
		
		case BlockTokenEventType::END:
#ifdef SKALANG_LOG_SYMBOL_TABLE
			std::cout << "\tBlock end : going up in nested symbol table hierarchy" << std::endl;
#endif
			assert(m_currentTable != nullptr);
			m_currentTable = &m_currentTable->parent();
			break;

		default:
			break;
	}
	return true;
}

bool ska::SymbolTable::matchFunction(FunctionTokenEvent& token) {
	//TODO gsl::not_null<...> pour m_currentTable
	assert(m_currentTable != nullptr);
			
	switch(token.type) {

		case FunctionTokenEventType::DECLARATION_PARAMETERS: {
            auto& symbol = m_currentTable->emplace(token.name, ExpressionType::FUNCTION); 

#ifdef SKALANG_LOG_SYMBOL_TABLE
            std::cout << "\t\tNew function : adding a nested symbol table" << std::endl;
#endif

            m_currentTable = &m_currentTable->createNested();
#ifdef SKALANG_LOG_SYMBOL_TABLE
            std::cout << "this function (" << token.name << ") has " << (token.node.size()) << " parameters with the following types : " << std::endl;
#endif
            auto currentArgList = std::vector<Symbol>{};
            for(auto index = 0u; index < token.node.size(); index++) {
                auto& param = token.node[index];
#ifdef SKALANG_LOG_SYMBOL_TABLE
                std::cout << token.node[index].asString() << " = " << ExpressionTypeSTR[static_cast<std::size_t>(param.type.value())] << std::endl;
#endif
                m_currentTable->emplace(param.asString(), param.type.value());
                currentArgList.push_back(param.type.value());
            }
            
            symbol.link(std::move(currentArgList), *m_currentTable);

            //Already handled with the variable declaration, here we juste create the function scope
            token.node.type = ExpressionType::FUNCTION;
        } break;

        case FunctionTokenEventType::DECLARATION_STATEMENT:
#ifdef SKALANG_LOG_SYMBOL_TABLE
			std::cout << "\t\tFunction end: going up in nested symbol table hierarchy" << std::endl;
#endif
            m_currentTable = &m_currentTable->parent();
        break;

		default:
		case FunctionTokenEventType::CALL: {
            const auto name = token.node.token.asString();
            const auto symbol = (*this)[name];
			if(symbol == nullptr) {
				throw std::runtime_error("Symbol function not found : " + name);
			}

			if(symbol->category != ExpressionType::FUNCTION) {
				throw std::runtime_error("Symbol \"" + name + "\" declared as normal variable but used as a function");
			}	
		} break;
	}
	
	return true;
}

bool ska::SymbolTable::match(VarTokenEvent& token) {
	assert(token.node.size() == 1);
	assert(m_currentTable != nullptr);
	
	switch(token.type) {
		case VarTokenEventType::DECLARATION: {
            const auto type = token.node[0].type.value();
			const auto name = token.node.token.asString();
            m_currentTable->emplace(name, type); 
#ifdef SKALANG_LOG_SYMBOL_TABLE	
			std::cout << "Matching new variable : " << name << " with type " << ExpressionTypeSTR[static_cast<std::size_t>(type)] << std::endl;
#endif
            if(type == ExpressionType::FUNCTION) {
                //token.node.token = Token { name, TokenType::IDENTIFIER };
            }   
        } break;

		default:
		case VarTokenEventType::AFFECTATION:
		case VarTokenEventType::USE: {
			const auto variableName = token.node.token.asString();
#ifdef SKALANG_LOG_SYMBOL_TABLE			
			std::cout << "Using variable : " << variableName  << std::endl;
#endif
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


ska::Symbol& ska::ScopedSymbolTable::emplace(std::string name, ExpressionType type) {
	m_symbols.emplace(name, Symbol { type });
    return m_symbols.at(std::move(name));
}

ska::ScopedSymbolTable& ska::ScopedSymbolTable::createNested() {
	m_children.push_back(std::make_unique<ska::ScopedSymbolTable>(*this));
	auto& lastChild = *m_children.back();
	//No bad memory access possible when unique_ptr are moved, that's why it's safe to return the address of contained item
	//even if we move the vector or if the vector moves its content automatically 
	return lastChild;
}


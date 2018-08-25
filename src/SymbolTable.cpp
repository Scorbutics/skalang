#include "SymbolTable.h"

#define SKALANG_LOG_SYMBOL_TABLE

ska::SymbolTable::SymbolTable(Observable<VarTokenEvent>& variableDeclarer, Observable<BlockTokenEvent>& scopeMaker, Observable<FunctionTokenEvent>& functionUser) :
	SubObserver<VarTokenEvent>(std::bind(&ska::SymbolTable::match, this, std::placeholders::_1), variableDeclarer),
	SubObserver<BlockTokenEvent>(std::bind(&ska::SymbolTable::nestedTable, this, std::placeholders::_1), scopeMaker),
	SubObserver<FunctionTokenEvent>(std::bind(&ska::SymbolTable::matchFunction, this, std::placeholders::_1), functionUser) {
		m_rootTable = std::make_unique<ScopedSymbolTable>();
		m_currentTable = m_rootTable.get();
}

bool ska::SymbolTable::nestedTable(BlockTokenEvent& event) {
	switch(event.type) {
		case BlockTokenEventType::START:
#ifdef SKALANG_LOG_SYMBOL_TABLE
			std::cout << "New block : adding a nested symbol table" << std::endl;
#endif
			m_currentTable = &m_currentTable->createNested();
			break;
		
		case BlockTokenEventType::END:
#ifdef SKALANG_LOG_SYMBOL_TABLE
			std::cout << "Block end : going up in nested symbol table hierarchy" << std::endl;
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
    const auto functionName = token.node.token.asString();
			
	switch(token.type) {
		case FunctionTokenEventType::DECLARATION: {
            //Already handled with the variable declaration, here we just add parameters types
            token.node.type = ExpressionType::FUNCTION;
        } break;

		default:
		case FunctionTokenEventType::CALL: {
            const auto symbol = (*this)[functionName];
			if(symbol == nullptr) {
				throw std::runtime_error("Symbol function not found : " + functionName);
			}

			if(symbol->category != ExpressionType::FUNCTION) {
				throw std::runtime_error("Symbol \"" + functionName + "\" declared as normal variable but used as a function");
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
                token.node[0].token = Token { name, TokenType::IDENTIFIER };
                
                auto symbol = (*this)[name];
                assert(symbol != nullptr);
                
#ifdef SKALANG_LOG_SYMBOL_TABLE
                std::cout << "this new variable is a function with the following parameters types : ";
#endif
                for(auto index = 0u; index < token.node[0].size() - 1; index++) {
                    auto& param = token.node[0][index];
#ifdef SKALANG_LOG_SYMBOL_TABLE
            std::cout  << ExpressionTypeSTR[static_cast<std::size_t>(param.type.value())] << std::endl;
#endif
                    symbol->add(param.type.value());
                }

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


void ska::ScopedSymbolTable::emplace(std::string name, ExpressionType type) {
	m_symbols.emplace(std::move(name), Symbol { type });
}

ska::ScopedSymbolTable& ska::ScopedSymbolTable::createNested() {
	m_children.push_back(std::make_unique<ska::ScopedSymbolTable>(*this));
	auto& lastChild = *m_children.back();
	//No bad memory access possible when unique_ptr are moved, that's why it's safe to return the address of contained item
	//even if we move the vector or if the vector moves its content automatically 
	return lastChild;
}


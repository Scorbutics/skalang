#include "SymbolTable.h"
#include "Parser.h"

//#define SKALANG_LOG_SYMBOL_TABLE

const ska::Symbol* ska::Symbol::operator[](const std::string& symbol) const {
	if (category.symbolTable() != nullptr && !category.symbolTable()->children().empty()) {
		auto& st = *category.symbolTable()->children()[0];
		return st[symbol];
	}
	return nullptr;
}

ska::Symbol* ska::Symbol::operator[](const std::string& symbol) {
	return const_cast<Symbol*>(static_cast<const Symbol*>(this)->operator[](symbol));
}

std::size_t ska::Symbol::size() const {
    return category.compound().size();
}

void ska::Symbol::link(std::vector<Symbol> subtypes, ScopedSymbolTable& table) {
	table.link(*this);
	category.link(table);
	//m_scopedTable = &table;
    
	for (const auto& t : subtypes) {
		category.add(std::move(t.category));
	}
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

bool ska::SymbolTable::matchReturn(ReturnTokenEvent& token) {
	auto actualNameSymbol = m_currentTable->parentSymbol();
	if (actualNameSymbol == nullptr) {
		throw std::runtime_error("bad user-defined return placing : custom return must be set in a named function-constructor");
	}

	m_currentTable = &m_currentTable->createNested();
#ifdef SKALANG_LOG_SYMBOL_TABLE
	std::cout << "\tReturn : nested named symbol table with name : " << actualNameSymbol->name << std::endl;
#endif
	for (auto index = 0u; index < token.node.size(); index++) {
		auto& field = token.node[index];
		auto& fieldValue = token.node[index][0];
		//const auto& symbolField = (*actualNameSymbol)[field[0].asString()];
		m_currentTable->emplace(field.asString(), fieldValue.type.value());
		//const auto fieldType = symbolField->category;
		//const auto valueName = field.asString();
		
		/*auto symbol = (*this)[valueName];
		if (symbol == nullptr) {
			
			symbol = (*this)[valueName];
		}*/
		
#ifdef SKALANG_LOG_SYMBOL_TABLE
		//std::cout << actualNameSymbol->name << " class has field " << valueName << " with type " << fieldType.asString() << std::endl;
#endif
	}
	m_currentTable = &m_currentTable->parent();
	return true;
}

bool ska::SymbolTable::matchFunction(FunctionTokenEvent& token) {
	//TODO gsl::not_null<...> pour m_currentTable
	assert(m_currentTable != nullptr);
			
	switch(token.type) {

		case FunctionTokenEventType::DECLARATION_PARAMETERS: {
			auto& symbol = m_currentTable->emplace(token.name, Type{ token.name, *m_currentTable, ExpressionType::FUNCTION });

#ifdef SKALANG_LOG_SYMBOL_TABLE
            std::cout << "\t\tNew function : adding a nested symbol table" << std::endl;
#endif
			auto* functionSymbolTable = &m_currentTable->createNested();
            m_currentTable = functionSymbolTable;
#ifdef SKALANG_LOG_SYMBOL_TABLE
            std::cout << "\t\tthis function (" << token.name << ") has " << (token.node.size() - 1) << " parameters : " << std::endl;
#endif
            auto currentArgList = std::vector<Symbol>{};
            for(auto index = 0u; index < token.node.size(); index++) {
                auto& param = token.node[index];
				auto name = param.asString();
				const auto typeStr = param.type.value().asString();

#ifdef SKALANG_LOG_SYMBOL_TABLE
                std::cout << "\t\t" << token.node[index].asString() << " = " << typeStr << std::endl;
#endif
				//Return type : mustn't be declared as a scope variable
				if (index != token.node.size() - 1) {
					m_currentTable->emplace(name, param.type.value());
				}

				currentArgList.push_back(Symbol{ std::move(name), param.type.value() });
            }
            
            symbol.link(std::move(currentArgList), *functionSymbolTable);

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
            const auto name = token.node[0].asString();
			auto* symbol = (*this)[name];
			auto* n = &token.node[0];
			auto* currentSymbolTable = symbol->category.symbolTable();
			while (n != nullptr && n->size() > 0 &&  !currentSymbolTable->children().empty()) {
				n = &(*n)[0];
				currentSymbolTable = currentSymbolTable->children()[0].get();
				const auto& fieldName = n->asString();
				auto* fieldSymbol = (*currentSymbolTable)[fieldName];
				std::cout << "field symbol : " << (fieldSymbol != nullptr ? fieldSymbol->category.asString() : "null") << std::endl;
				symbol = fieldSymbol;
			}

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
            auto type = token.node[0].type.value();
			const auto name = token.node.asString();
            m_currentTable->emplace(name, std::move(type)); 
#ifdef SKALANG_LOG_SYMBOL_TABLE	
			std::cout << "Matching new variable : " << name << " with type " << type.asString() << std::endl;
#endif
        } break;

		default:
		case VarTokenEventType::AFFECTATION:
		case VarTokenEventType::USE: {
			const auto variableName = token.node.asString();
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


ska::Symbol& ska::ScopedSymbolTable::emplace(std::string name, Type type) {
	assert(!name.empty());
	assert(type != ExpressionType::VOID);
	m_symbols.emplace(name, Symbol { name, std::move(type) });
    return m_symbols.at(std::move(name));
}

ska::ScopedSymbolTable& ska::ScopedSymbolTable::createNested() {
	m_children.push_back(std::make_unique<ska::ScopedSymbolTable>(*this));
	auto& lastChild = *m_children.back();
	//No bad memory access possible when unique_ptr are moved, that's why it's safe to return the address of contained item
	//even if we move the vector or if the vector moves its content automatically 
	return lastChild;
}


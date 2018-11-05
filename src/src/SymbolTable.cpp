#include "LoggerConfigLang.h"
#include "SymbolTable.h"
#include "Parser.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::SymbolTable)
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

void ska::Symbol::link(std::vector<Symbol> subtypes, ScopedSymbolTable& table) {
	table.link(*this);
	//m_category.link(table);
	//m_scopedTable = &table;
    
	for (const auto& t : subtypes) {
		m_category.add(t.getType());
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
	const auto actualNameSymbol = m_currentTable->parentSymbol();
	if (actualNameSymbol == nullptr) {
		throw std::runtime_error("bad user-defined return placing : custom return must be set in a named function-constructor");
	}

	m_currentTable = &m_currentTable->createNested();
	SLOG(ska::LogLevel::Info) << "\tReturn : nested named symbol table with name : " << actualNameSymbol->getName();
	for (auto index = 0u; index < token.rootNode().size(); index++) {
		auto& field = token.rootNode()[index];
		auto& fieldValue = token.rootNode()[index][0];
		//const auto& symbolField = (*actualNameSymbol)[field[0].asString()];
		m_currentTable->emplace(field.asString(), fieldValue.type().value());
		//const auto fieldType = symbolField->category;
		//const auto valueName = field.asString();
		
		/*const auto symbol = (*this)[valueName];
		*/
		
		//SLOG(ska::LogLevel::Debug) << actualNameSymbol->name << " class has field " << valueName << " with type " << fieldType.asString();
	}
	m_currentTable = &m_currentTable->parent();
	return true;
}

bool ska::SymbolTable::matchFunction(const FunctionTokenEvent& token) {
	//TODO gsl::not_null<...> pour m_currentTable
	assert(m_currentTable != nullptr);
	switch(token.type()) {

		case FunctionTokenEventType::DECLARATION_PARAMETERS: {
			auto& symbol = m_currentTable->emplace(token.name(), Type{ token.name(), ExpressionType::FUNCTION });
			SLOG(ska::LogLevel::Info) << "\t\tNew function : adding a nested symbol table";
			auto* functionSymbolTable = &m_currentTable->createNested();
            m_currentTable = functionSymbolTable;
			SLOG(ska::LogLevel::Info) << "\t\tthis function (" << token.name() << ") has " << (token.rootNode().size() - 1) << " parameters : ";
            auto currentArgList = std::vector<Symbol>{};
			auto index = 0u;
			for(auto& param : token) {
				if (index == token.rootNode().size() - 1) {
					continue;
				}
				param->add(std::make_unique<ASTNode>());
				auto name = param->asString();
				assert(!param->type().has_value());
				SLOG(ska::LogLevel::Debug) << "\t\t" << name;
				currentArgList.push_back(m_currentTable->emplace(std::move(name), ska::Type{}));
				
				index++;
            }
            
            symbol.link(std::move(currentArgList), *functionSymbolTable);
			SLOG(ska::LogLevel::Info) << "\t\tfunction type : " << symbol.getType().asString();
            //Already handled with the variable declaration, here we just create the function scope
            //token.rootNode().type() = ExpressionType::FUNCTION;
        } break;

        case FunctionTokenEventType::DECLARATION_STATEMENT:
			SLOG(ska::LogLevel::Debug) << "\t\tFunction end: going up in nested symbol table hierarchy";
            m_currentTable = &m_currentTable->parent();
        break;

		default:
		case FunctionTokenEventType::CALL: {
            const auto name = token.rootNode()[0].asString();
			auto* n = &token.rootNode()[0];
			auto* currentSymbolTable = m_currentTable;
			assert(currentSymbolTable != nullptr);
			const Symbol* symbol = nullptr;
			while (n != nullptr && n->size() > 0 && !currentSymbolTable->children().empty()) {
				n = &(*n)[0];
				currentSymbolTable = currentSymbolTable->children()[0].get();
				const auto& fieldName = n->asString();
				auto* fieldSymbol = (*currentSymbolTable)[fieldName];
				SLOG(ska::LogLevel::Info) << "field symbol : " << (fieldSymbol != nullptr ? fieldSymbol->getType().asString() : "null");
                symbol = fieldSymbol;
			}

			if(symbol == nullptr) {
				symbol = (*this)[name];
				if (symbol == nullptr) {
					throw std::runtime_error("Symbol function not found : " + name);
				}
			}

			if(symbol->getType() != ExpressionType::FUNCTION) {
				throw std::runtime_error("Symbol \"" + name + "\" declared as variable of type \"" + symbol->getType().asString() + "\" but used as a function");
			}	
		} break;
	}
	
	return true;
}

bool ska::SymbolTable::match(const VarTokenEvent& token) {
	assert(token.rootNode().size() == 1);
	assert(m_currentTable != nullptr);
	
	switch(token.type()) {
		case VarTokenEventType::DECLARATION: {			
			assert(token.rootNode().size() > 0);
			const auto type = token.rootNode()[0].type();
			const auto name = token.rootNode().asString();
			SLOG(ska::LogLevel::Info) << "Matching new variable : " << name << " with an undefined type";
			m_currentTable->emplace(name, /*type.has_value() ? type.value() :*/ ska::Type{});
        } break;

		default:
		case VarTokenEventType::AFFECTATION:
		case VarTokenEventType::USE: {
			const auto variableName = token.rootNode().asString();
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
        auto symbol = Symbol { name, type, *this };
		SLOG(ska::LogLevel::Debug) << "\tSymbol \"" << name << "\" \"" <<  symbol.getType().asString() << "\"";
        if(m_symbols.find(name) == m_symbols.end()) {
            m_symbols.emplace(name, std::move(symbol));
        } else {
            m_symbols.at(name) = std::move(symbol);
        }
    }

    auto& s = m_symbols.at(name);
	SLOG(ska::LogLevel::Debug) << "\tSymbol Inserted \"" << name << "\" \"" << s.getType().asString() << "\"";
    return s;
}

ska::ScopedSymbolTable& ska::ScopedSymbolTable::createNested() {
	m_children.push_back(std::make_unique<ska::ScopedSymbolTable>(*this));
	auto& lastChild = *m_children.back();
	//No bad memory access possible when unique_ptr are moved, that's why it's safe to return the address of contained item
	//even if we move the vector or if the vector moves its content automatically 
	return lastChild;
}


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
	
	switch(token.type) {
		case FunctionTokenEventType::DECLARATION:
		//Already handled with the variable declaration
		token.node.type = ExpressionType::FUNCTION;
		break;

		default:
		case FunctionTokenEventType::CALL: {
			const auto functionName = token.node.token.asString();
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

ska::ExpressionType ska::SymbolTable::crossTypes(char op, ExpressionType type1, ExpressionType type2) {
	constexpr auto TypeMapSize = 7;

	static int typeMapOperatorPlus[TypeMapSize][TypeMapSize] = { 
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 3, 4, 5, 0 },
		{ 0, 0, 0, 4, 4, 0, 0 },
		{ 0, 0, 0, 5, 0, 5, 0 },
		{ 0, 0, 0, 0, 0, 0, 6 }
	};

	static int typeMapOperatorMinus[TypeMapSize][TypeMapSize] = { 
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 3, 4, 0, 0 },
		{ 0, 0, 0, 4, 4, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 }
	};

	static int typeMapOperatorMul[TypeMapSize][TypeMapSize] = { 
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 3, 4, 5, 0 },
		{ 0, 0, 0, 4, 4, 0, 0 },
		{ 0, 0, 0, 5, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 }
	};

	static int typeMapOperatorDiv[TypeMapSize][TypeMapSize] = { 
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 3, 4, 0, 0 },
		{ 0, 0, 0, 4, 4, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 }
	};

	int (*selectedTypeMap)[TypeMapSize];

	switch(op) {
		case '-':
			selectedTypeMap = typeMapOperatorMinus;
			break;
		case '+':
			selectedTypeMap = typeMapOperatorPlus;
			break;
		case '/':
			selectedTypeMap = typeMapOperatorDiv;
			break;
		case '*':
			selectedTypeMap = typeMapOperatorMul;
			break;

		default: {
			std::cout << "Unknown operator \"" << op << "\", returning by default first type (of index " << 
				static_cast<std::size_t>(type1) << ") without checking type map" << std::endl;  
		} return type1;
	}

	const auto typeIdResult = selectedTypeMap[static_cast<std::size_t>(type1)][static_cast<std::size_t>(type2)];
	if(typeIdResult == 0) {
		auto ss = std::stringstream {};
		ss << "Unable to use operator \"" << op << "\" on types of index " << static_cast<std::size_t>(type1) << " and " << static_cast<std::size_t>(type2);
		throw std::runtime_error(ss.str()); 
	}

	return static_cast<ExpressionType>(typeIdResult);

}

ska::ExpressionType ska::SymbolTable::calculateNodeExpressionType(ASTNode& node) const {
	if(node.op.has_value() && node.op.value() != Operator::LITERAL) {
		const auto& op = node.op.value();
		switch(op) {
			case Operator::FUNCTION_DECLARATION:
				return ExpressionType::FUNCTION;

			case Operator::FUNCTION_CALL: {
				const auto functionName = node.token.asString();
				const auto symbol = (*this)[functionName];
				return symbol == nullptr ? ExpressionType::VOID : symbol->category;
			}
			
			case Operator::FIELD_ACCESS:
				//TODO
				throw std::runtime_error("TODO");
			
			case Operator::BINARY: {
				assert(node.size() == 2 && !node.token.asString().empty());
				const auto type1 = getExpressionType(node[0]);
				const auto type2 = getExpressionType(node[1]);
				return crossTypes(node.token.asString()[0], type1, type2);
			}

			case Operator::VARIABLE_AFFECTATION:
			case Operator::UNARY:
				assert(node.size() == 1);
				return getExpressionType(node[0]);

			default:
				return ExpressionType::VOID;
		}
	}

	const auto& token = node.token;
	switch(token.type()) {
		case TokenType::SYMBOL:
		case TokenType::SPACE:
		case TokenType::RANGE:
		case TokenType::DOT_SYMBOL:
			return ExpressionType::VOID;

		case TokenType::STRING:
			return ExpressionType::STRING;

		case TokenType::DIGIT:
			return ExpressionType::FLOAT;
		
		case TokenType::IDENTIFIER: {
			const auto symbol = (*this)[token.asString()];
			return symbol == nullptr ? ExpressionType::VOID : symbol->category;
		}
		default:
			break;
	}

	return ExpressionType::VOID;

}

ska::ExpressionType ska::SymbolTable::getExpressionType(ASTNode& node) const {
	if(node.type.has_value()) {
		return node.type.value();
	}

	const auto expressionType = calculateNodeExpressionType(node);
	node.type = expressionType;
	return expressionType;
}

bool ska::SymbolTable::match(VarTokenEvent& token) {
	assert(token.node.size() == 1);
	assert(m_currentTable != nullptr);
	
	switch(token.type) {
		case VarTokenEventType::DECLARATION:
			m_currentTable->emplace(std::move(token.node.token.asString()), getExpressionType(token.node)); 
#ifdef SKALANG_LOG_SYMBOL_TABLE	
			std::cout << "Matching new variable : " << token.node.token.asString() << std::endl;
#endif
		break;

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
			const auto tokenNodeExpressionType = getExpressionType(token.node[0]);
			if(token.type == VarTokenEventType::AFFECTATION && symbol->category != tokenNodeExpressionType) {
				throw std::runtime_error("The symbol \"" + variableName + "\" has already been declared as a " + 
						(symbol->category != ExpressionType::FUNCTION ? "variable": "function" ) + " but is now wanted to be a " +
						(symbol->category == ExpressionType::FUNCTION ? "variable" : "function"));
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


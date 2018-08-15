//#include <iostream>
#include "SymbolTable.h"

ska::SymbolTable::SymbolTable(Observable<VarTokenEvent>& variableDeclarer, Observable<BlockTokenEvent>& scopeMaker) :
	SubObserver<VarTokenEvent>(std::bind(&ska::SymbolTable::match, this, std::placeholders::_1), variableDeclarer),
	SubObserver<BlockTokenEvent>(std::bind(&ska::SymbolTable::nestedTable, this, std::placeholders::_1), scopeMaker) {
		m_rootTable = std::make_unique<ScopedSymbolTable>();
		m_currentTable = m_rootTable.get();
}

bool ska::SymbolTable::nestedTable(BlockTokenEvent& event) {
	switch(event.type) {
		case BlockTokenEventType::START:
			std::cout << "New block : adding a nested symbol table" << std::endl;
			m_currentTable = &m_currentTable->createNested();
			break;
		
		case BlockTokenEventType::END:
			std::cout << "Block end : going up in nested symbol table hierarchy" << std::endl;
			assert(m_currentTable != nullptr);
			m_currentTable = &m_currentTable->parent();
			break;

		default:
			break;
	}
	return true;
}


bool ska::SymbolTable::match(VarTokenEvent& token) {
	assert(token.node.size() == 2);
	assert(m_currentTable != nullptr);

	m_currentTable->emplace(std::move(token.node[0].asString()), (token.node[1].op.has_value() ? token.node[1].op.value() : Operator::VARIABLE_DECLARATION)); 
	
	std::cout << "Matching new variable : " << token.node[0].asString() << std::endl;
	
	return true;
}


ska::ScopedSymbolTable& ska::ScopedSymbolTable::parent() {
	return m_parent;
}


void ska::ScopedSymbolTable::emplace(std::string name, Operator op) {
	m_symbols.emplace(std::move(name), Symbol { op });
}

ska::ScopedSymbolTable& ska::ScopedSymbolTable::createNested() {
	m_children.push_back(std::make_unique<ska::ScopedSymbolTable>(*this));
	auto& lastChild = *m_children.back();
	//No bad memory access possible when unique_ptr are moved, that's why it's safe to return the address of contained item
	//even if we move the vector 
	return lastChild;
}


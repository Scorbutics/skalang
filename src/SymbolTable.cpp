#include <iostream>
#include "SymbolTable.h"

ska::SymbolTable::SymbolTable(Observable<VarTokenEvent>& variableDeclarer) :
    SubObserver<VarTokenEvent>(std::bind(&ska::SymbolTable::match, this, std::placeholders::_1), variableDeclarer) {
}

bool ska::SymbolTable::match(VarTokenEvent& token) {
    //TODO
    assert(token.node.size() == 2);

    std::cout
    << "Symbol table updated : name = "
    << token.node[0].asString()
    << "\tcategory = "
    << (token.node[1].op.has_value() ? (token.node[1].op == Operator::FUNCTION_DECLARATION ? "function" : "variable") : "")
    << std::endl;
    return true;
}

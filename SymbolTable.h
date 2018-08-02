#pragma once
#include <Utils/SubObserver.h>
#include <Utils/Observable.h>

#include "AST.h"
#include "VarTokenEvent.h"

namespace ska {
    struct Symbol {
        std::string category;
    };

	class SymbolTable :
        public SubObserver<VarTokenEvent> {

        using ASTNodePtr = std::unique_ptr<ska::ASTNode>;

    public:
		SymbolTable(Observable<VarTokenEvent>& variableDeclarer);
		~SymbolTable() = default;

    private:
        bool match(VarTokenEvent&);

        std::unordered_map<std::string, Symbol> m_symbols;
        std::unique_ptr<SymbolTable> m_parent;
	};
}

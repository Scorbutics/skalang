#pragma once
#include <Utils/SubObserver.h>

#include "ExpressionTokenEvent.h"
#include "ExpressionType.h"
#include "VarTokenEvent.h"
#include "FunctionTokenEvent.h"

namespace ska {
	class SymbolTable;
    class Parser;

    class SemanticTypeChecker :
    public SubObserver<VarTokenEvent>,
    public SubObserver<FunctionTokenEvent> {
		public:
			SemanticTypeChecker(Parser& parser, const SymbolTable& symbolTable);
            ~SemanticTypeChecker() = default;
		private:
	        bool matchVariable(const VarTokenEvent& token); 		
            bool matchFunction(const FunctionTokenEvent& token);
            const SymbolTable& m_symbols;
    };

}

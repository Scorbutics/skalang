#pragma once
#include <Utils/SubObserver.h>
#include "Operator.h"
#include "ExpressionType.h"

#include "VarTokenEvent.h"
#include "FunctionTokenEvent.h"
#include "ExpressionTokenEvent.h"
#include "ReturnTokenEvent.h"

namespace ska {
    class ASTNode;
    class SymbolTable;
    class Parser;

    class TypeBuilder : 
	    public SubObserver<ExpressionTokenEvent>,
		public SubObserver<FunctionTokenEvent>,
		public SubObserver<VarTokenEvent>,
		public SubObserver<ReturnTokenEvent> {
        public:
            TypeBuilder(Parser& parser, const SymbolTable& symbolTable);
            ~TypeBuilder() = default;
            
        private:
            bool matchVariable(VarTokenEvent& token) const;
            bool matchFunction(FunctionTokenEvent& event) const;
            bool matchExpression(ExpressionTokenEvent& event) const;
			bool matchReturn(ReturnTokenEvent& event) const;

            const SymbolTable& m_symbols;

    };
    
}
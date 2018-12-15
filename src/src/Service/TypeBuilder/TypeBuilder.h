#pragma once
#include <Utils/SubObserver.h>
#include "NodeValue/Operator.h"
#include "NodeValue/ExpressionType.h"

#include "Event/VarTokenEvent.h"
#include "Event/FunctionTokenEvent.h"
#include "Event/ExpressionTokenEvent.h"
#include "Event/ReturnTokenEvent.h"
#include "Event/ArrayTokenEvent.h"

namespace ska {
    class ASTNode;
    class SymbolTable;
    class Parser;

    class TypeBuilder : 
	    public SubObserver<ExpressionTokenEvent>,
		public SubObserver<FunctionTokenEvent>,
		public SubObserver<VarTokenEvent>,
		public SubObserver<ReturnTokenEvent>,
		public SubObserver<ArrayTokenEvent> {
        public:
            TypeBuilder(Parser& parser, const SymbolTable& symbolTable);
            ~TypeBuilder() = default;
            
        private:
            bool matchVariable(VarTokenEvent& token) const;
            bool matchFunction(FunctionTokenEvent& event) const;
            bool matchExpression(ExpressionTokenEvent& event) const;
			bool matchReturn(ReturnTokenEvent& event) const;
			bool matchArray(ArrayTokenEvent& event) const;

            const SymbolTable& m_symbols;

    };
    
}
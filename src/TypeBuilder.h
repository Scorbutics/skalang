#pragma once
#include "Operator.h"
#include "ExpressionType.h"

#include "VarTokenEvent.h"
#include "FunctionTokenEvent.h"
#include "ExpressionTokenEvent.h"



namespace ska {
    class ASTNode;
    class SymbolTable;

    class TypeBuilder {
        public:
            TypeBuilder(const SymbolTable& symbolTable);
            ~TypeBuilder() = default;
            
        private:
            bool matchVariable(VarTokenEvent& token) const;
            bool matchFunction(FunctionTokenEvent& event) const;
            bool matchExpression(ExpressionTokenEvent& event) const;

            const SymbolTable& m_symbols;

    };
    
}

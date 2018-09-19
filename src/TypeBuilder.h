#pragma once
#include "Operator.h"
#include "ExpressionType.h"

namespace ska {
    class ASTNode;

    class TypeBuilder {
        public:
            TypeBuilder(const SymbolTable& symbolTable);
            ~TypeBuilder() = default;
            
        private:
            Type calculateExpressionType(ASTNode& node) const;
            bool matchVariable(VarTokenEvent& token) const;
            bool matchFunction(FunctionTokenEvent& event) const;
            
            const SymbolTable& m_symbols;

    };

    template <Operator O>
    struct TypeBuilderOperator {
        static Type build(ASTNode& node) {
            return ExpressionType::VOID;
        }
    };

    template<>
    struct TypeBuilderOperator<Operator::FUNCTION_DECLARATION> {
        static Type build(ASTNode& node);
    };
}

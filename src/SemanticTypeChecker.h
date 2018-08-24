#pragma once
#include <Utils/SubObserver.h>

#include "ExpressionTokenEvent.h"
#include "ExpressionType.h"
#include "VarTokenEvent.h"

namespace ska {
	class SymbolTable;

    class SemanticTypeChecker :
	public SubObserver<ExpressionTokenEvent>,
    public SubObserver<VarTokenEvent> {
		public:
			SemanticTypeChecker(Observable<ExpressionTokenEvent>& expressionDeclarer, Observable<VarTokenEvent>& varDeclarer);
			void setSymbolTable(const SymbolTable& symbolTable);
            ~SemanticTypeChecker() = default;
		private:
			ExpressionType getExpressionType(ASTNode& node) const;
        	ExpressionType calculateNodeExpressionType(ASTNode& node) const;
		    static ExpressionType crossTypes(char op, ExpressionType type1, ExpressionType type2);
            bool matchExpression(ExpressionTokenEvent& event);
	        bool matchVariable(VarTokenEvent& token); 		
            const SymbolTable* m_symbols = nullptr;
	};

}

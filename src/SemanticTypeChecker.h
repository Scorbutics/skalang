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
	public SubObserver<ExpressionTokenEvent>,
    public SubObserver<VarTokenEvent>,
    public SubObserver<FunctionTokenEvent> {
		public:
			SemanticTypeChecker(Parser& parser);
			void setSymbolTable(const SymbolTable& symbolTable);
            ~SemanticTypeChecker() = default;
		private:
			Type getExpressionType(ASTNode& node) const;
			Type calculateNodeExpressionType(ASTNode& node) const;
            bool matchExpression(ExpressionTokenEvent& event);
	        bool matchVariable(VarTokenEvent& token); 		
            bool matchFunction(FunctionTokenEvent& token);
            const SymbolTable* m_symbols = nullptr;
	};

}

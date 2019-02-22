#pragma once
#include "Container/sorted_observable.h"

#include "NodeValue/Operator.h"
#include "NodeValue/ExpressionType.h"

#include "Event/VarTokenEvent.h"
#include "Event/FunctionTokenEvent.h"
#include "Event/ExpressionTokenEvent.h"
#include "Event/ReturnTokenEvent.h"
#include "Event/ArrayTokenEvent.h"
#include "Event/BridgeTokenEvent.h"

namespace ska {
    class ASTNode;
    class SymbolTable;
    class StatementParser;

    class TypeBuilder : 
	    public subobserver_priority_queue<ExpressionTokenEvent>,
		public subobserver_priority_queue<FunctionTokenEvent>,
		public subobserver_priority_queue<VarTokenEvent>,
		public subobserver_priority_queue<ReturnTokenEvent>,
		public subobserver_priority_queue<ArrayTokenEvent>,
		public subobserver_priority_queue<BridgeTokenEvent> {
        public:
            TypeBuilder(StatementParser& parser, const SymbolTable& symbolTable);
            virtual ~TypeBuilder() = default;
            
        private:
            bool matchVariable(VarTokenEvent& token) const;
            bool matchFunction(FunctionTokenEvent& event) const;
            bool matchExpression(ExpressionTokenEvent& event) const;
			bool matchReturn(ReturnTokenEvent& event) const;
			bool matchArray(ArrayTokenEvent& event) const;
			bool matchBridge(BridgeTokenEvent& event) const;

            const SymbolTable& m_symbols;
			StatementParser& m_parser;

    };
    
}

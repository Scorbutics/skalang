#pragma once
#include <Utils/SubObserver.h>

#include "VarTokenEvent.h"
#include "ReturnTokenEvent.h"
#include "FunctionTokenEvent.h"
#include "ExpressionTokenEvent.h"

namespace ska {
	class ASTNode;
	class SymbolTable;
	class Parser;

	class SymbolTableTypeUpdater :
		public SubObserver<ExpressionTokenEvent>,
		public SubObserver<FunctionTokenEvent>,
		public SubObserver<VarTokenEvent>,
		public SubObserver<ReturnTokenEvent> {
	public:
		SymbolTableTypeUpdater(Parser& parser, SymbolTable& symbolTable);
		~SymbolTableTypeUpdater() = default;

	private:
		bool matchVariable(const VarTokenEvent& token);
		bool matchFunction(const FunctionTokenEvent& event);
		bool matchExpression(const ExpressionTokenEvent& event);
		bool matchReturn(const ReturnTokenEvent& event);

		void updateType(const ASTNode& node);

		SymbolTable& m_symbols;
	};
}
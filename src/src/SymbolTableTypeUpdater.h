#pragma once
#include <Utils/SubObserver.h>

#include "VarTokenEvent.h"
#include "FunctionTokenEvent.h"
#include "ExpressionTokenEvent.h"

namespace ska {
	class ASTNode;
	class SymbolTable;
	class Parser;

	class SymbolTableTypeUpdater :
		public SubObserver<ExpressionTokenEvent>,
		public SubObserver<FunctionTokenEvent>,
		public SubObserver<VarTokenEvent> {
	public:
		SymbolTableTypeUpdater(Parser& parser, SymbolTable& symbolTable);
		~SymbolTableTypeUpdater() = default;

	private:
		bool matchVariable(const VarTokenEvent& token);
		bool matchFunction(const FunctionTokenEvent& event);
		bool matchExpression(const ExpressionTokenEvent& event);

		void updateType(const ASTNode& node);

		SymbolTable& m_symbols;
	};
}
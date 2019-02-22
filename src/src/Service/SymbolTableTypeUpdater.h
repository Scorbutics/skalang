#pragma once
#include "Container/sorted_observable.h"

#include "Event/VarTokenEvent.h"

namespace ska {
	class ASTNode;
	class SymbolTable;
	class StatementParser;

	class SymbolTableTypeUpdater :
		public subobserver_priority_queue<VarTokenEvent> {
	public:
		SymbolTableTypeUpdater(StatementParser& parser, SymbolTable& symbolTable);
		~SymbolTableTypeUpdater() = default;

	private:
		bool matchVariable(const VarTokenEvent& token);

		void updateType(const ASTNode& node);
		
        SymbolTable& m_symbols;
	};
}

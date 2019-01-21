#pragma once
#include <Utils/SubObserver.h>

#include "Event/VarTokenEvent.h"

namespace ska {
	class ASTNode;
	class SymbolTable;
	class StatementParser;

	class SymbolTableTypeUpdater :
		public SubObserver<VarTokenEvent>{
	public:
		SymbolTableTypeUpdater(StatementParser& parser, SymbolTable& symbolTable);
		~SymbolTableTypeUpdater() = default;

	private:
		bool matchVariable(const VarTokenEvent& token);

		void updateType(const ASTNode& node);
		
        SymbolTable& m_symbols;
	};
}

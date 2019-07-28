#pragma once
#include "Container/sorted_observable.h"

#include "Event/VarTokenEvent.h"

namespace ska {
	class ASTNode;
	class SymbolTable;
	class StatementParser;

	class SymbolTableUpdater :
		public subobserver_priority_queue<VarTokenEvent> {
	public:
		SymbolTableUpdater(StatementParser& parser);
		~SymbolTableUpdater() = default;

	private:
		bool matchVariable(VarTokenEvent& token);

		void updateType(const ASTNode& node, SymbolTable& s);
		void updateNode(ASTNode& node, const std::string& variableName, const SymbolTable& s);
	};
}

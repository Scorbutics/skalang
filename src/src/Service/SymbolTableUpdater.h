#pragma once
#include "Container/sorted_observable.h"

#include "Event/VarTokenEvent.h"
#include "Event/FunctionTokenEvent.h"

namespace ska {
	class ASTNode;
	class SymbolTable;
	class StatementParser;

	class SymbolTableUpdater :
		public subobserver_priority_queue<VarTokenEvent>,
		public subobserver_priority_queue<FunctionTokenEvent> {
	public:
		SymbolTableUpdater(StatementParser& parser);
		~SymbolTableUpdater() = default;

	private:
		bool matchVariable(VarTokenEvent& token);
		bool matchFunction(FunctionTokenEvent& token);

		void updateType(const ASTNode& node, SymbolTable& s);
		void updateNode(ASTNode& node, const std::string& variableName, SymbolTable& s);
	};
}

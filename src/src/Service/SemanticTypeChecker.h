#pragma once
#include <Utils/SubObserver.h>

#include "Event/ExpressionTokenEvent.h"
#include "NodeValue/ExpressionType.h"
#include "Event/VarTokenEvent.h"
#include "Event/FunctionTokenEvent.h"
#include "Event/ReturnTokenEvent.h"
#include "Event/IfElseTokenEvent.h"

namespace ska {
	class SymbolTable;
    class StatementParser;

    class SemanticTypeChecker :
    public SubObserver<VarTokenEvent>,
    public SubObserver<FunctionTokenEvent>,
	public SubObserver<ArrayTokenEvent>,
    public SubObserver<ReturnTokenEvent>,
	public SubObserver<IfElseTokenEvent> {
    public:
        SemanticTypeChecker(StatementParser& parser, const SymbolTable& symbols);
        ~SemanticTypeChecker() = default;
    private:
		static bool childrenHasReturnOnAllControlPath(const ASTNode& node);
		static bool statementHasReturnOnAllControlPath(const ASTNode& node);

        bool matchArray(const ArrayTokenEvent& token);
        bool matchVariable(const VarTokenEvent& token); 		
        bool matchFunction(const FunctionTokenEvent& token);
        bool matchReturn(const ReturnTokenEvent& token);
		bool matchIfElse(const IfElseTokenEvent& token);
        const SymbolTable& m_symbols;
    };

}

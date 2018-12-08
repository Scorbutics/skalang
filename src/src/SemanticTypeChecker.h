#pragma once
#include <Utils/SubObserver.h>

#include "ExpressionTokenEvent.h"
#include "ExpressionType.h"
#include "VarTokenEvent.h"
#include "FunctionTokenEvent.h"
#include "ReturnTokenEvent.h"

namespace ska {
	class SymbolTable;
    class Parser;

    class SemanticTypeChecker :
    public SubObserver<VarTokenEvent>,
    public SubObserver<FunctionTokenEvent>,
	public SubObserver<ArrayTokenEvent>,
    public SubObserver<ReturnTokenEvent> {
    public:
        SemanticTypeChecker(Parser& parser, const SymbolTable& symbols);
        ~SemanticTypeChecker() = default;
    private:
        bool matchArray(const ArrayTokenEvent& token);
        bool matchVariable(const VarTokenEvent& token); 		
        bool matchFunction(const FunctionTokenEvent& token);
        bool matchReturn(const ReturnTokenEvent& token);
        const SymbolTable& m_symbols;
    };

}

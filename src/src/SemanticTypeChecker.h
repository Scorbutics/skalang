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
    public SubObserver<VarTokenEvent>,
    public SubObserver<FunctionTokenEvent>,
	public SubObserver<ArrayTokenEvent> {
		public:
			SemanticTypeChecker(Parser& parser);
            ~SemanticTypeChecker() = default;
		private:
			bool matchArray(const ArrayTokenEvent& token);
	        bool matchVariable(const VarTokenEvent& token); 		
            bool matchFunction(const FunctionTokenEvent& token);
    };

}

#pragma once
#include <functional>
#include <vector>
#include <unordered_map>
#include <Data/Events/EventDispatcher.h>
#include "Interpreter/BridgeFunction.h"
#include "Utils/Observable.h"
#include "Event/VarTokenEvent.h"

namespace ska {
    class SymbolTable;

    struct ScriptBridge {
        std::string name;
        ASTNodePtr node;
    };

	class ScriptBinding : 
        public Observable<VarTokenEvent> {
	public:
		ScriptBinding() = default;
		virtual ~ScriptBinding() = default;

		ScriptBridge buildScriptBridge(Script& script, const std::string& scriptName, std::vector<BridgeFunctionPtr> functions);
	};
}

#pragma once

#include "BytecodeScriptCache.h"

namespace ska {
	namespace bytecode {

		struct ScriptGen {
			ScriptGen() = default;
			ScriptGen(ScriptGenerationService service) : service(std::move(service)) {}
			ScriptGen(ScriptCache& scriptCache, const std::string& fullName, std::vector<Token> tokens);
			ScriptGen(ScriptCache& scriptCache, ScriptAST& scriptAST, const std::string& fullName);

			ScriptGen(const ScriptGen&) = delete;
			ScriptGen(ScriptGen&&) = default;

			ScriptGen& operator=(const ScriptGen&) = delete;
			ScriptGen& operator=(ScriptGen&&) = default;

			ScriptAST& astScript();
			void memoryFromBridge(std::vector<BridgeFunctionPtr> bindings);

			~ScriptGen() = default;

			ScriptGenerationService service;
			ScriptGenerationOutput output;
		private:
			std::optional<ScriptAST> m_ast;
		};

	}
}

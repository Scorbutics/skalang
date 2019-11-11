#pragma once

#include "BytecodeScriptCache.h"

namespace ska {
	namespace bytecode {

		struct Script {
			Script() = default;
			Script(ScriptGenerationService service) : service(std::move(service)) {}
			Script(ScriptCache& scriptCache, const std::string& fullName, std::vector<Token> tokens);
			Script(ScriptCache& scriptCache, ScriptAST& scriptAST, const std::string& fullName);

			Script(const Script&) = delete;
			Script(Script&&) = default;

			Script& operator=(const Script&) = delete;
			Script& operator=(Script&&) = default;

			ScriptAST& astScript();
			void memoryFromBridge(std::vector<BridgeFunctionPtr> bindings);

			~Script() = default;

			ScriptGenerationService service;
			ScriptGenerationOutput output;
		private:
			std::optional<ScriptAST> m_ast;
		};

	}
}

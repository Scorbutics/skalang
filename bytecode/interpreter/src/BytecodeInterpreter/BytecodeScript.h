#pragma once
#include "Generator/Value/BytecodeScriptGen.h"
#include "BytecodeInterpreter/Value/BytecodeExecutor.h"
#include "BytecodeInterpreter/Value/BytecodeInterpreterTypes.h"

namespace ska {
	namespace bytecode {
		class ScriptExecution;
		class Generator;
		class Interpreter;

		class Script {
		public:
			Script(ScriptCache& cache, const std::string& fullName, std::vector<ska::Token> tokens) :
				m_cache(cache),
				m_serviceGen(m_cache, fullName, std::move(tokens)) {}

			void generate(Generator& generator) {
				m_serviceGen.generate(generator);
			}

			std::unique_ptr<Executor> execute(Interpreter& interpreter);

			PlainMemoryTable createMemory() {
				//TODO !!!
				return {};
			}

			std::string name() const { return astScript().name(); }

			Script(Script&&) = default;
			Script(const Script&) = delete;
			Script& operator=(const Script&) = delete;
			Script& operator=(Script&&) = default;

			ScriptAST& astScript() { return m_serviceGen.astScript(); }
			const ScriptAST& astScript() const { return m_serviceGen.astScript(); }

			void memoryFromBridge(Interpreter& interpreter, std::vector<BridgeFunctionPtr> bindings);

			RuntimeMemory memoryField(const std::string& symbol);

			Value getSymbol(const Symbol& symbol) const;

		private:
			Value findBytecodeMemoryFromSymbol(const Symbol& symbol) const;
			const Symbol& findSymbolFromString(const std::string& key) const;
			ScriptCache& m_cache;
			ScriptGen m_serviceGen;
			ScriptExecution* m_execution;
		};
	}
}

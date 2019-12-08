#pragma once
#include "Generator/Value/BytecodeScriptGeneration.h"
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
				m_serviceGen(m_cache, std::move(tokens), fullName) {}

			void generate(Generator& generator) {
				m_serviceGen.generate(m_cache, generator);
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

			ScriptAST astScript() const { return m_serviceGen.program(); }

			void memoryFromBridge(Interpreter& interpreter, std::vector<BridgeFunctionPtr> bindings);

			RuntimeMemory memoryField(const std::string& symbol);

			Operand getSymbol(const Symbol& symbol) const;

		private:
			Operand findBytecodeMemoryFromSymbol(const Symbol& symbol) const;
			const Symbol& findSymbolFromString(const std::string& key) const;
			ScriptCache& m_cache;
			ScriptGeneration m_serviceGen;
			ScriptExecution* m_execution = nullptr;
		};
	}
}

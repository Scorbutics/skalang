#pragma once
#include "Generator/Value/BytecodeScriptGen.h"
#include "BytecodeInterpreter/Value/BytecodeExecutor.h"

namespace ska {
	namespace bytecode {
		class ScriptExecution;
		class Generator;
		class Interpreter;

		class Script {
		public:
			Script(ScriptCache& cache, const std::string& fullName, std::vector<ska::Token> tokens) :
				m_cache(cache),
				m_generation { GenerationOutput { m_cache } },
				m_serviceGen(m_cache, fullName, std::move(tokens)) {}

			void generate(Generator& generator);
			std::unique_ptr<Executor> execute(Interpreter& interpreter);

			PlainMemoryTable createMemory() { return {}; }
			std::string name() const { return m_generation.back().name().toString(); }

			Script(Script&&) = default;
			Script(const Script&) = delete;
			Script& operator=(const Script&) = delete;
			Script& operator=(Script&&) = default;

			ScriptAST& astScript() { return m_serviceGen.astScript(); }
			void memoryFromBridge(std::vector<BridgeFunctionPtr> bindings) { return m_serviceGen.memoryFromBridge(std::move(bindings)); }

			//ScriptGen& generation() { return m_generation; }
			//ScriptExecution& execution() { return m_execution; }

		private:
			bool m_generated = false;
			unsigned int m_id = std::numeric_limits<unsigned int>::max();
			ScriptCache& m_cache;
			GenerationOutput m_generation;
			ScriptGen m_serviceGen;
			ScriptExecution* m_execution;
		};
	}
}

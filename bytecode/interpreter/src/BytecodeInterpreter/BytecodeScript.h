#pragma once
#include "Generator/Value/BytecodeScriptGen.h"
#include "BytecodeInterpreter/BytecodeScriptExecution.h"

namespace ska {
	namespace bytecode {

		class Script {
		public:
			Script(ScriptGen generation, ScriptExecution execution) :
				m_generation(std::move(generation)),
				m_execution(std::move(execution)) {
			}

			PlainMemoryTable createMemory() { return {}; }
			std::string name() const { return m_generation.output.name().toString(); }

			Script(Script&&) = default;
			Script(const Script&) = delete;
			Script& operator=(const Script&) = delete;
			Script& operator=(Script&&) = default;

			ScriptGen& generation() { return m_generation; }
			ScriptExecution& execution() { return m_execution; }

		private:
			ScriptGen m_generation;
			ScriptExecution m_execution;
		};
	}
}

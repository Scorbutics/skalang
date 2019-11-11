#pragma once
#include "Generator/Value/BytecodeScript.h"
#include "BytecodeInterpreter/BytecodeScriptExecution.h"

namespace ska {
	namespace bytecode {

		class ScriptPackage {
		public:
			ScriptPackage(Script script, ScriptExecution execution) :
				m_script(std::move(script)),
				m_execution(std::move(execution)) {
			}

			ScriptPackage(ScriptPackage&&) = default;
			ScriptPackage(const ScriptPackage&) = delete;
			ScriptPackage& operator=(const ScriptPackage&) = delete;
			ScriptPackage& operator=(ScriptPackage&&) = default;

		private:
			Script m_script;
			ScriptExecution m_execution;
		};
	}
}

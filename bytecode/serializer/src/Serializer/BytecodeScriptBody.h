#pragma once
#include <cstddef>

#include "Generator/Value/BytecodeInstruction.h"
#include "Generator/Value/BytecodeExport.h"

namespace ska {
	namespace bytecode {
		struct ScriptBody {
			ScriptBody(const std::vector<std::string>& natives) : m_natives(natives) {}

			const std::vector<std::string>& natives() const { return m_natives; }
			
			std::vector<Instruction> instructions {};
			std::vector<ExportSymbol> exports {};

		private:
			const std::vector<std::string>& m_natives;
		};
	}
}

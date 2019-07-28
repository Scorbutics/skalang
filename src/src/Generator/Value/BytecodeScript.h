#pragma once
#include <string>
#include <optional>
#include <unordered_map>

#include "Base/Values/MovableNonCopyable.h"
#include "Interpreter/Value/Script.h"
#include "Generator/Value/BytecodeValue.h"

namespace ska {
	class ASTNode;
	namespace bytecode {
		class Script :
			public MovableNonCopyable {
			using SymbolUIDContainer = std::unordered_map<const Symbol*, std::size_t>;
		public:
			Script(ska::Script& script);
			
			Script(Script&&) = default;
			Script& operator=(Script&&) = default;

			ska::Script program() { return ska::Script{ *m_script }; }

			Register queryNextRegister(Type type);
			Value queryVariableOrValue(const ASTNode& node);

		private:
			std::size_t m_register = 0;
			ska::ScriptHandle* m_script{};
			SymbolUIDContainer m_variables;
			std::size_t m_variableCount = 0;
		};
	}
}

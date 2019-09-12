#pragma once
#include <string>
#include <optional>
#include <unordered_map>

#include "Base/Values/MovableNonCopyable.h"
#include "NodeValue/ScriptAST.h"
#include "BytecodeValue.h"

namespace ska {
	class ASTNode;
	namespace bytecode {
		class ScriptGenerationService :
			public MovableNonCopyable {

		public:
			ScriptGenerationService(ska::ScriptAST& script);

			ScriptGenerationService(ScriptGenerationService&&) = default;
			ScriptGenerationService& operator=(ScriptGenerationService&&) = default;

			ska::ScriptAST program() { return ska::ScriptAST{ *m_script }; }

			Register queryNextRegister();

			~ScriptGenerationService() override = default;

		private:
			std::size_t m_register = 0;
			ska::ScriptHandleAST* m_script{};
		};
	}
}

#pragma once
#include <cassert>
#include "Value/BytecodeCellGroup.h"

namespace ska {
	class ASTNode;
	struct ScriptHandle;
	class Script;
	
	class BytecodeGenerationContext {
	public:
		BytecodeGenerationContext() = default;
		explicit BytecodeGenerationContext(ScriptHandle& program);
		explicit BytecodeGenerationContext(Script& program);
		BytecodeGenerationContext(ScriptHandle& program, const ASTNode& node);

		const ASTNode& pointer() { assert(m_pointer != nullptr); return *m_pointer; }
		ScriptHandle& program() { assert(m_program != nullptr); return *m_program; }

		BytecodeCell cellFromValue(BytecodeCommand commandToGenerate);

	private:
		ScriptHandle* m_program {};
		const ASTNode* m_pointer {};
		std::size_t m_bytecodeGenerationIndex = 0;
	};
}

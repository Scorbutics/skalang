#pragma once
#include <cassert>
#include "Value/BytecodeCellGroup.h"
#include "Value/BytecodeScript.h"

namespace ska {
	class ASTNode;
	
	class BytecodeGenerationContext {
	public:
		BytecodeGenerationContext() = default;
		BytecodeGenerationContext(BytecodeScript& script);
		BytecodeGenerationContext(BytecodeScript& script, const ASTNode& node);

		const ASTNode& pointer() { assert(m_pointer != nullptr); return *m_pointer; }
		BytecodeScript& script() { assert(m_script != nullptr); return *m_script; }

		BytecodeCell cellFromValue(BytecodeCommand commandToGenerate);

	private:
		BytecodeScript* m_script{};
		const ASTNode* m_pointer {};
		std::size_t m_bytecodeGenerationIndex = 0;
	};
}

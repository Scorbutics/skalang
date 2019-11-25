#pragma once
#include <queue>
#include <string>
#include <optional>
#include <unordered_map>

#include "Base/Values/MovableNonCopyable.h"
#include "NodeValue/ScriptAST.h"
#include "BytecodeOperand.h"
#include "UniqueSymbolGetter.h"
#include "BytecodeSymbolInfo.h"

namespace ska {
	namespace bytecode {
		struct SymbolWithInfo {
			const Symbol* symbol = nullptr;
			const SymbolInfo* info = nullptr;
		};

		static inline bool operator<(const SymbolWithInfo& lhs, const SymbolWithInfo& rhs) {
			return lhs.info->priority > rhs.info->priority;
		}

	}
}

namespace ska {
	class ASTNode;
	class ScriptAST;
	namespace bytecode {
		class ScriptGenCache;
		class ScriptGenerationService :
			public MovableNonCopyable,
			private UniqueSymbolGetter<'V'>{
			using VariableGetter = UniqueSymbolGetter<'V'>;
		public:
			ScriptGenerationService() = default;
			ScriptGenerationService(std::size_t scriptIndex, ScriptAST& script);
			ScriptGenerationService(ScriptGenCache& cache, const ScriptAST& script);

			ScriptGenerationService(ScriptGenerationService&&) = default;
			ScriptGenerationService& operator=(ScriptGenerationService&&) = default;

			ska::ScriptAST program() const { return ska::ScriptAST{ *m_script }; }
			const ASTNode& rootASTNode() const { return m_script->rootNode(); }
			const std::string& name() const { return m_script->name(); }

			Register queryNextRegister();
			Operand querySymbolOrOperand(const ASTNode& node);
			Operand querySymbol(const Symbol& symbol);
			std::optional<Operand> getSymbol(const Symbol& symbol) const;
			std::vector<Operand> generateExportedSymbols(std::priority_queue<SymbolWithInfo> symbolsInfo) const;

			~ScriptGenerationService() override = default;

		private:
			std::size_t m_index = std::numeric_limits<std::size_t>::max();
			std::size_t m_register = 0;
			ska::ScriptHandleAST* m_script{};
		};
	}
}

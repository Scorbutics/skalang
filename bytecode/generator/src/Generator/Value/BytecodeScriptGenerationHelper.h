#pragma once
#include <queue>
#include <string>
#include <optional>
#include <unordered_map>

#include "NodeValue/ScriptASTPtr.h"
#include "NodeValue/Token.h"
#include "Base/Values/MovableNonCopyable.h"
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
			return lhs.info->childIndex > rhs.info->childIndex;
		}

	}
}

namespace ska {
	class ASTNode;
	class ScriptAST;
	class StatementParser;
	struct ScriptHandleAST;

	namespace bytecode {
		class ScriptCache;

		class ScriptGenerationHelper :
			public MovableNonCopyable,
			private UniqueSymbolGetter<'V'>{
			using VariableGetter = UniqueSymbolGetter<'V'>;
		public:
			ScriptGenerationHelper(std::size_t index, const ScriptAST& script);
			ScriptGenerationHelper(ScriptCache& cache, const ScriptAST& script);
			ScriptGenerationHelper(ScriptCache& cache, StatementParser& parser, const std::string& scriptName, std::vector<Token> tokens);

			ScriptGenerationHelper(ScriptGenerationHelper&&) = default;
			ScriptGenerationHelper& operator=(ScriptGenerationHelper&&) = default;

			ska::ScriptAST program() const;
			const ASTNode& rootASTNode() const;
			const std::string& name() const;
			std::size_t id() const { return m_index; }

			ska::ScriptASTPtr useImport(const std::string& scriptImported);
			Register queryNextRegister();
			Operand querySymbolOrOperand(const ASTNode& node);
			Operand querySymbol(const Symbol& symbol);
			std::optional<Operand> getSymbol(const Symbol& symbol) const;
			std::vector<Operand> generateExportedSymbols(std::priority_queue<SymbolWithInfo> symbolsInfo) const;

			~ScriptGenerationHelper() override = default;

		private:
			std::size_t m_index = std::numeric_limits<std::size_t>::max();
			std::size_t m_register = 0;
			ska::ScriptHandleAST* m_script{};
		};
	}
}

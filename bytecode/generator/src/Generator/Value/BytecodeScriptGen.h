#pragma once

#include "BytecodeGenerationOutput.h"
#include "NodeValue/ScriptAST.h"

namespace ska {
	namespace bytecode {
		class Generator;

		struct ScriptGen {
			ScriptGen(ScriptCache& scriptCache, std::size_t scriptIndex);
			ScriptGen(ScriptCache& scriptCache, const std::string& fullName, std::vector<Token> tokens);
			ScriptGen(ScriptCache& scriptCache, ScriptAST& scriptAST, const std::string& fullName);

			ScriptGen(const ScriptGen&) = delete;
			ScriptGen(ScriptGen&&) = default;

			ScriptGen& operator=(const ScriptGen&) = delete;
			ScriptGen& operator=(ScriptGen&&) = default;

			ScriptAST& astScript();
			const ScriptAST& astScript() const;

			std::optional<Operand> getSymbol(const Symbol& symbol) const;

			auto id() const { return m_id; }
			void generate(Generator& generator);
			const ScriptGeneration& generated() const { assert(m_generated); return m_cache.at(m_id); }
			GenerationOutput& wholeGenerated() { assert(m_generated); return m_cache; }

			~ScriptGen() = default;

		private:
			static ScriptGeneration& AddScript(ska::bytecode::ScriptCache& cache, std::vector<ska::Token> tokens, const std::string& name);
			ScriptGeneration& m_service;
			unsigned int m_id = std::numeric_limits<unsigned int>::max();
			bool m_generated = false;
			ScriptCache& m_cache;
			mutable std::optional<ScriptAST> m_ast;
		};

	}
}

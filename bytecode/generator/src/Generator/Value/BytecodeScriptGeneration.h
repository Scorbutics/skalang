#pragma once
#include <ostream>
#include <vector>
#include <optional>
#include "NodeValue/Operator.h"
#include "NodeValue/Type.h"
#include "BytecodeInstruction.h"
#include "BytecodeSymbolInfo.h"
#include "BytecodeInstructionOutput.h"
#include "BytecodeScriptGenerationHelper.h"
#include "NodeValue/ScriptAST.h"

namespace ska {
	class ASTNode;
	namespace bytecode {
		class Generator;
		class ScriptCache;

		using InstructionPack = std::vector<Instruction>;

		std::ostream& operator<<(std::ostream& stream, const InstructionPack&);

		class ScriptGeneration {
			friend class InstructionOutput;
		public:
			ScriptGeneration(ScriptGenerationHelper origin, InstructionOutput instruction = {});
			ScriptGeneration(ScriptCache& cache, std::vector<ska::Token> tokens, const std::string& name);

			ScriptGeneration(ScriptGeneration&&) = default;
			ScriptGeneration(const ScriptGeneration&) = delete;
			ScriptGeneration& operator=(ScriptGeneration&&) = default;
			ScriptGeneration& operator=(const ScriptGeneration&) = delete;
			~ScriptGeneration() = default;

			const ASTNode& rootASTNode() const;

			const Instruction& operator[](std::size_t index) const { return m_generated[index]; }
			Instruction& operator[](std::size_t index) { return m_generated[index]; }

			const std::vector<Operand>& exportedSymbols() const { return m_exports; }
			void setExportedSymbols(std::vector<Operand> symbols) { m_exports = std::move(symbols); };

			std::optional<std::vector<Operand>> generateExportedSymbols(std::priority_queue<SymbolWithInfo> symbolsInfo) const {
				if(m_exports.empty()) {
					return m_origin.generateExportedSymbols(std::move(symbolsInfo));
				}
				return std::optional<std::vector<Operand>>{};
			}

			std::optional<Operand> getSymbol(const Symbol& symbol) const;
			ScriptGenerationHelper& helper() { return m_origin; }

			ScriptAST program() const { return m_origin.program(); }
			bool empty() const { return m_generated.empty(); }
			std::size_t size() const { return m_generated.size(); }
			std::size_t id() const { return m_origin.id(); }
			const std::string& name() const { return m_origin.name(); }

			auto begin() const { return m_generated.begin(); }
			auto end() const { return m_generated.end(); }

			void generate(ScriptCache& cache, Generator& generator);
			void generate(ScriptCache& cache, InstructionOutput instructions);

		private:
			friend std::ostream& operator<<(std::ostream& stream, const ScriptGeneration&);
			friend bool operator==(const ScriptGeneration& left, const ScriptGeneration& right);
			friend bool operator!=(const ScriptGeneration& left, const ScriptGeneration& right);
			//void push(ScriptGeneration output);

			ScriptGenerationHelper m_origin;
			std::vector<Operand> m_exports;
			InstructionOutput m_generated;
		};

		std::ostream& operator<<(std::ostream& stream, const ScriptGeneration&);
	}
}

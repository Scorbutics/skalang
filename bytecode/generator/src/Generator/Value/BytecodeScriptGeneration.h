#pragma once
#include <ostream>
#include <vector>
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
			ScriptGeneration(ScriptGenerationHelper origin, InstructionOutput instruction = {}) :
				m_origin(std::move(origin)),
				m_generated(std::move(instruction)) {
			}

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

			//TODO lazy-optimize with exportedSymbols ?
			std::vector<Operand> generateExportedSymbols(std::priority_queue<SymbolWithInfo> symbolsInfo) const { return m_origin.generateExportedSymbols(std::move(symbolsInfo)); }

			ScriptASTPtr useImport(const std::string& scriptImported);
			Operand querySymbolOrOperand(const ASTNode& node) { return m_origin.querySymbolOrOperand(node); }
			Operand querySymbol(const Symbol& symbol) { return m_origin.querySymbol(symbol); }
			std::optional<Operand> getSymbol(const Symbol& symbol) const { return m_origin.getSymbol(symbol); }
			Register queryNextRegister() { return m_origin.queryNextRegister(); }

			ScriptAST program() { return m_origin.program(); }
			bool empty() const { return m_generated.empty(); }
			std::size_t size() const { return m_generated.size(); }
			std::size_t id() const { return m_origin.id(); }
			const std::string& name() const { return m_origin.name(); }

			auto begin() const { return m_generated.begin(); }
			auto end() const { return m_generated.end(); }

			void generate(ScriptCache& cache, Generator& generator);

		private:
			friend std::ostream& operator<<(std::ostream& stream, const ScriptGeneration&);

			void push(ScriptGeneration output);

			ScriptGenerationHelper m_origin;
			std::vector<Operand> m_exports;
			InstructionOutput m_generated;
		};

		std::ostream& operator<<(std::ostream& stream, const ScriptGeneration&);
	}
}

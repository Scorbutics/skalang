#pragma once
#include <ostream>
#include <vector>
#include "NodeValue/Operator.h"
#include "NodeValue/Type.h"
#include "BytecodeInstruction.h"
#include "BytecodeSymbolInfo.h"
#include "BytecodeInstructionOutput.h"

namespace ska {
	namespace bytecode {
		class ScriptGenerationHelper;

		using InstructionPack = std::vector<Instruction>;

		std::ostream& operator<<(std::ostream& stream, const InstructionPack&);

		class ScriptGenerationOutput {
			friend class InstructionOutput;
		public:
			ScriptGenerationOutput(std::size_t index, ScriptGenerationHelper& origin, InstructionOutput instruction = {}) :
				m_index(index),
				m_origin(origin),
				m_generated(std::move(instruction)) {
			}

			ScriptGenerationHelper& origin() { return m_origin; }

			const Instruction& operator[](std::size_t index) const { return m_generated[index]; }
			Instruction& operator[](std::size_t index) { return m_generated[index]; }

			const std::vector<Operand>& exportedSymbols() const { return m_exports; }
			void setExportedSymbols(std::vector<Operand> symbols) { m_exports = std::move(symbols); };

			bool empty() const { return m_generated.empty(); }
			std::size_t size() const { return m_generated.size(); }
			std::size_t id() const { return m_index; }

			auto begin() const { return m_generated.begin(); }
			auto end() const { return m_generated.end(); }

		private:
			friend std::ostream& operator<<(std::ostream& stream, const ScriptGenerationOutput&);

			void push(ScriptGenerationOutput output);

			std::size_t m_index = std::numeric_limits<std::size_t>::max();
			std::vector<Operand> m_exports;
			InstructionOutput m_generated;
			ScriptGenerationHelper& m_origin;
		};

		std::ostream& operator<<(std::ostream& stream, const ScriptGenerationOutput&);
	}
}

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
		class ScriptGenerationService;

		using InstructionPack = std::vector<Instruction>;

		std::ostream& operator<<(std::ostream& stream, const InstructionPack&);

		class ScriptGenerationOutput {
			friend class InstructionOutput;
		public:
			ScriptGenerationOutput(ScriptGenerationService& origin, InstructionOutput instruction) :
				m_origin(origin),
				m_generated(std::move(instruction)) {
			}

			ScriptGenerationOutput(ScriptGenerationService& origin) :
				m_origin(origin) {
			}

			ScriptGenerationService& origin() { return m_origin; }

			const Instruction& operator[](std::size_t index) const { return m_generated[index]; }
			Instruction& operator[](std::size_t index) { return m_generated[index]; }

			const std::vector<Value>& exportedSymbols() const { return m_exports; }
			void setExportedSymbols(std::vector<Value> symbols) { m_exports = std::move(symbols); };

			bool empty() const { return m_generated.empty(); }

		private:
			friend std::ostream& operator<<(std::ostream& stream, const ScriptGenerationOutput&);

			void push(ScriptGenerationOutput output);
			Value packAsValue() const;

			std::vector<Value> m_exports;
			InstructionOutput m_generated;
			ScriptGenerationService& m_origin;
		};

		std::ostream& operator<<(std::ostream& stream, const ScriptGenerationOutput&);
	}
}

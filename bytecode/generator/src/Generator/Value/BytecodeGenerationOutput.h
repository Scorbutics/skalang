#pragma once
#include <ostream>
#include <vector>
#include "NodeValue/Operator.h"
#include "NodeValue/Type.h"
#include "BytecodeInstruction.h"
#include "BytecodeSymbolInfo.h"

namespace ska {
	namespace bytecode {
		using InstructionPack = std::vector<Instruction>;

		std::ostream& operator<<(std::ostream& stream, const InstructionPack&);

		class GenerationOutput {
		public:
			GenerationOutput(Instruction instruction, SymbolInfo symbol = {}) :
				m_pack(InstructionPack { std::move(instruction) }) {
				m_value = packAsValue();
			}

			GenerationOutput() = default;

			GenerationOutput(Value value) :
				m_value(std::move(value)) {
			}

			void push(GenerationOutput value);

			bool empty() const {
				return m_pack.empty() && m_value.empty();
			}

			auto size() const { return m_pack.size();	}

			Value name() const { return m_pack.empty() ? m_value : m_pack[0].dest(); }

			auto begin() const { return m_pack.begin(); }
			auto end() const { return m_pack.end(); }

			auto begin() { return m_pack.begin(); }
			auto end() { return m_pack.end(); }

			Value value() const {	return m_value.empty() ? packAsValue() : m_value;	}

			const Instruction& operator[](std::size_t index) const { return m_pack[index]; }
			Instruction& operator[](std::size_t index) { return m_pack[index]; }

		private:
			friend std::ostream& operator<<(std::ostream& stream, const GenerationOutput&);

			Value packAsValue() const;
			 InstructionPack m_pack;
			 Value m_value;
		};

		std::ostream& operator<<(std::ostream& stream, const GenerationOutput&);
	}
}

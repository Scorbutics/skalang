#pragma once
#include <ostream>
#include <vector>
#include "NodeValue/Operator.h"
#include "NodeValue/Type.h"
#include "BytecodeInstruction.h"

namespace ska {
	namespace bytecode {
		using InstructionPack = std::vector<Instruction>;

		std::ostream& operator<<(std::ostream& stream, const InstructionPack&);

		class GenerationOutput {
		public:
			GenerationOutput(Instruction instruction) :
				m_pack(InstructionPack { std::move(instruction) }) {
				m_value = packAsValue();
			}

			GenerationOutput(InstructionPack pack) :
				m_pack(std::move(pack)) {
				m_value = packAsValue();
			}

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

			Value value() const {	return m_value.empty() ? packAsValue() : m_value;	}

		private:
			friend std::ostream& operator<<(std::ostream& stream, const GenerationOutput&);

			Value packAsValue() const;
			 InstructionPack m_pack;
			 Value m_value;
		};

		std::ostream& operator<<(std::ostream& stream, const GenerationOutput&);
	}
}

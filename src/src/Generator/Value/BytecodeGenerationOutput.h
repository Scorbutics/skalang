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
			}

			GenerationOutput(InstructionPack pack) :
				m_pack(std::move(pack)) {
			}

			GenerationOutput(Value value) :
				m_value(std::move(value)) {
			}

			void push(GenerationOutput value) {
				m_pack.insert(m_pack.end(), std::make_move_iterator(value.m_pack.begin()), std::make_move_iterator(value.m_pack.end()));
			}

			bool empty() const {
				return m_pack.empty() && m_value.empty();
			}

			Value value() const {
				return m_pack.empty() ? m_value : m_pack.back().dest();
			}

			const InstructionPack& pack() const {
				return m_pack;
			}

			/*
			void transferValueToPack(Register r) {
				if (!m_value.empty()) {
					m_pack.push_back(Instruction{ Command::MOV, std::move(r), std::move(m_value) });
					m_value = {};
				}
			}
			*/

		private:
			 InstructionPack m_pack;
			 Value m_value;
		};
	}
}

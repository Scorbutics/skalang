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
			GenerationOutput(InstructionPack pack) :
				m_pack(std::move(pack)) {
			}

			GenerationOutput(Value value) :
				m_value(std::move(value)) {
			}

			void push(GenerationOutput value) {
				value.transferValueToPack();
				transferValueToPack();
				//TODO move
				m_pack.insert(m_pack.end(), value.m_pack.begin(), value.m_pack.end());
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

		private:
			void transferValueToPack() {
				if(!m_value.empty()) {
					m_pack.push_back(Instruction{Command::MOV, std::move(m_value)});
					m_value = {};
				}
			}

			 InstructionPack m_pack;
			 Value m_value;
		};
	}
}

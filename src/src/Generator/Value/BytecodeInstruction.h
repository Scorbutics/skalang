#pragma once
#include <variant>
#include "NodeValue/Type.h"
#include "Generator/Value/BytecodeValue.h"

namespace ska {
	namespace bytecode {

		class Instruction {
		public:
			Instruction(Command command, Value dest, Value left = {}, Value right = {}) :
				m_command(std::move(command)),
				m_dest(std::move(dest)),
				m_left(std::move(left)),
				m_right(std::move(right)) {
			}

			Instruction() = default;

			auto command() const { return m_command; }
			const auto& left() const { return m_left;	}
			const auto& right() const { return m_right;	}
			const auto& dest() const { return m_dest; }

			friend std::ostream& operator<<(std::ostream& stream, const Instruction&);

		private:
			Command m_command = Command::NOP;
			Value m_dest;
			Value m_left;
			Value m_right;
			Type m_type;
		};

		std::ostream& operator<<(std::ostream& stream, const Instruction&);

	}
}

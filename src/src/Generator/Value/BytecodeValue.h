#pragma once
#include <variant>
#include "NodeValue/Type.h"
#include "Generator/BytecodeCommand.h"

/*
Opcode : enum value (MOV, SUB, ADD, MUL, DIV...)

Value :
	- std::string
	- Type

Register : value

Instruction :
	- opcode
	- left value
	- right value
	- dest value
	(- debug infos :
		- line
		- column) -> not for now...



GenerationOutput :
	- std::vector<Instruction>
*/

namespace ska {
	namespace bytecode {

		struct Value {
			std::string content;

			bool empty() const {
				return content.empty();
			}
		};

		using Register = Value;
	}
}

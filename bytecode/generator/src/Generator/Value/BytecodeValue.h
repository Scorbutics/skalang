#pragma once
#include <variant>
#include "NodeValue/Type.h"
#include "Generator/BytecodeCommand.h"
#include "NodeValue/StringShared.h"
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
	class ASTNode;

	namespace bytecode {

		using ValueVariant = std::variant<std::size_t, long, bool, double, StringShared>;

		enum class ValueType {
			PURE,
			VAR,
			REG,
			LBL,
			EMPTY
		};

		struct Value {
			Value() = default;

			Value(const ASTNode& node);

			Value(ValueVariant var, ValueType type = ValueType::PURE) :
				content(std::move(var)),
				type(type){
			}

			ValueVariant content;
			ValueType type = ValueType::EMPTY;

			bool empty() const {
				return type == ValueType::EMPTY;
			}

			std::string toString() const;

			template <class T>
			T& as() { return std::get<T>(content); }

			template <class T>
			const T& as() const { return std::get<T>(content); }

		};

		using Register = Value;
	}
}

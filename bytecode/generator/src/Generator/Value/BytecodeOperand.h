#pragma once
#include <variant>
#include <memory>
#include <tuple>
#include <unordered_map>
#include "NodeValue/Type.h"
#include "Generator/BytecodeCommand.h"
#include "Runtime/Value/StringShared.h"
#include "Runtime/Value/ScriptVariableRef.h"

/*
Opcode : enum (MOV, SUB, ADD, MUL, DIV...)

Operand :
	- std::string
	- Type

Register : operand

Instruction :
	- opcode
	- left operand
	- right operand
	- dest operand
	(- debug infos :
		- line
		- column) -> not for now...



InstructionOutput :
	- std::vector<Instruction>
*/

namespace ska {
	class ASTNode;

	namespace bytecode {
		using OperandVariant = std::variant<ScriptVariableRef, long, bool, double, StringShared>;

		enum class OperandType {
			PURE,
			VAR,
			REG,
			EMPTY
		};

		static constexpr const char* OperandTypeSTR[] = {
			"PURE",
			"VAR",
			"REG",
			""
		};

		static inline std::ostream& operator<<(std::ostream& stream, const OperandType& val) {
			stream << OperandTypeSTR[static_cast<std::size_t>(val)];
			return stream;
		}

		struct Operand {
			template<typename T, typename VARIANT_T>
			struct isVariantMember;

			template<typename T, typename... ALL_T>
			struct isVariantMember<T, std::variant<ALL_T...>>
				: public std::disjunction<std::is_same<T, ALL_T>...> {};
			
			Operand() = default;

			Operand(const ASTNode& node);

			Operand(OperandVariant var, OperandType type = OperandType::PURE) :
				m_content(std::move(var)),
				m_type(type){
			}

			bool empty() const { return m_type == OperandType::EMPTY; }
			const auto& content() const { return m_content; }
			const auto type() const { return m_type; }

			std::string toString() const;

			template <class T>
			T& as() { return std::get<T>(m_content); }

			template <class T>
			const T& as() const { return std::get<T>(m_content); }

			template <class Converted>
			static constexpr bool is_member_of_values() {
				return isVariantMember<Converted, OperandVariant>::value;
			}

		private:
			friend bool operator==(const Operand& lhs, const Operand& rhs);

			OperandVariant m_content;
			OperandType m_type = OperandType::EMPTY;
		};

		using Register = Operand;

		bool operator==(const Operand& lhs, const Operand& rhs);

	}


}

namespace std {
	template<>
	struct hash<ska::bytecode::Operand> {
		size_t operator()(const ska::bytecode::Operand & x) const {
			const size_t h1 = hash<std::string>()(x.toString());
    		const size_t h2 = hash<std::size_t>()(static_cast<std::size_t>(x.type()));
    		return h1 ^ (h2 << 1);
		}
	};
}

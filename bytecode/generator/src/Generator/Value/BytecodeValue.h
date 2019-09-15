#pragma once
#include <variant>
#include <memory>
#include <tuple>
#include <unordered_map>
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



ScriptGenerationOutput :
	- std::vector<Instruction>
*/

namespace ska {
	class ASTNode;

	namespace bytecode {
		struct VariableRef {			
			std::size_t variable = 0;
		};

		struct ScriptVariableRef {
			std::size_t variable = 0;
			std::size_t script = 0;
		};

		/*
		using VariableRefIndexMap = std::unordered_map<std::size_t, std::size_t>;
		using InstanceReferences = std::shared_ptr<VariableRefIndexMap>;
		*/
		using ValueVariant = std::variant<VariableRef, long, bool, double, StringShared>;

		enum class ValueType {
			PURE,
			VAR,
			REG,
			EMPTY
		};

		struct Value {
			
			template<typename T, typename VARIANT_T>
			struct isVariantMember;

			template<typename T, typename... ALL_T>
			struct isVariantMember<T, std::variant<ALL_T...>>
				: public std::disjunction<std::is_same<T, ALL_T>...> {};
			
			Value() = default;

			Value(const ASTNode& node);

			Value(ValueVariant var, ValueType type = ValueType::PURE) :
				m_content(std::move(var)),
				m_type(type){
			}

			bool empty() const { return m_type == ValueType::EMPTY; }
			const auto& content() const { return m_content; }
			const auto type() const { return m_type; }

			std::string toString() const;

			template <class T>
			T& as() { return std::get<T>(m_content); }

			template <class T>
			const T& as() const { return std::get<T>(m_content); }

			template <class Converted>
			static constexpr bool is_member_of_values() {
				return isVariantMember<Converted, ValueVariant>::value;
			}

		private:
			friend bool operator==(const Value& lhs, const Value& rhs);

			ValueVariant m_content;
			ValueType m_type = ValueType::EMPTY;
		};

		using Register = Value;

		bool operator==(const Value& lhs, const Value& rhs);
		bool operator==(const VariableRef& lhs, const VariableRef& rhs);
		bool operator==(const ScriptVariableRef& lhs, const ScriptVariableRef& rhs);

		inline std::ostream& operator<<(std::ostream& stream, const ScriptVariableRef& var) {
			stream << var.variable;
			return stream;
		}
	}


}

namespace std {
	template<>
	struct hash<ska::bytecode::ScriptVariableRef> {
		size_t operator()(const ska::bytecode::ScriptVariableRef & x) const {
			const size_t h1 = hash<std::size_t>()(x.script);
			const size_t h2 = hash<std::size_t>()(x.variable);
			return h1 ^ (h2 << 1);
		}
	};

	template<>
	struct hash<ska::bytecode::VariableRef> {
		size_t operator()(const ska::bytecode::VariableRef& x) const {
			return hash<std::size_t>()(x.variable);
		}
	};

	template<>
	struct hash<ska::bytecode::Value> {
		size_t operator()(const ska::bytecode::Value & x) const {
			const size_t h1 = hash<std::string>()(x.toString());
    		const size_t h2 = hash<std::size_t>()(static_cast<std::size_t>(x.type()));
    		return h1 ^ (h2 << 1);
		}
	};
}

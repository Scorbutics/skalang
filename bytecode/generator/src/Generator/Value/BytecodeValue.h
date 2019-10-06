#pragma once
#include <variant>
#include <memory>
#include <tuple>
#include <unordered_map>
#include "NodeValue/Type.h"
#include "Generator/BytecodeCommand.h"
#include "NodeValue/StringShared.h"
#include "Runtime/Value/ScriptVariableRef.h"

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

		/*
		using VariableRefIndexMap = std::unordered_map<std::size_t, std::size_t>;
		using InstanceReferences = std::shared_ptr<VariableRefIndexMap>;
		*/
		using ValueVariant = std::variant<ScriptVariableRef, long, bool, double, StringShared>;

		enum class ValueType {
			PURE,
			VAR,
			REG,
			EMPTY
		};

		static constexpr const char* ValueTypeSTR[] = {
			"PURE",
			"VAR",
			"REG",
			""
		};

		static inline std::ostream& operator<<(std::ostream& stream, const ValueType& val) {
			stream << ValueTypeSTR[static_cast<std::size_t>(val)];
			return stream;
		}

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

	}


}

namespace std {
	template<>
	struct hash<ska::bytecode::Value> {
		size_t operator()(const ska::bytecode::Value & x) const {
			const size_t h1 = hash<std::string>()(x.toString());
    		const size_t h2 = hash<std::size_t>()(static_cast<std::size_t>(x.type()));
    		return h1 ^ (h2 << 1);
		}
	};
}

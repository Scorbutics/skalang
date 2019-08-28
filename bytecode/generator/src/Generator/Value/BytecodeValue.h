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



GenerationOutput :
	- std::vector<Instruction>
*/

namespace ska {
	class ASTNode;

	namespace bytecode {
		using VariableRef = std::tuple<std::size_t>;
		using VariableRefIndexMap = std::unordered_map<std::size_t, std::size_t>;
		using InstanceReferences = std::shared_ptr<VariableRefIndexMap>;
		using ValueVariant = std::variant<VariableRef, long, bool, double, StringShared>;

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
				m_content(std::move(var)),
				m_type(type){
			}

			bool empty() const { return m_type == ValueType::EMPTY; }
			const auto& content() const { return m_content; }
			const auto type() const { return m_type; }

			std::string toString() const;
			std::string referencesToString() const;

			template <class T>
			T& as() { return std::get<T>(m_content); }

			template <class T>
			const T& as() const { return std::get<T>(m_content); }

			const VariableRefIndexMap& references() const {
				return *ref;
			}

			void addReference(std::size_t indexReference) {
				if(ref == nullptr) {
					ref = std::make_shared<VariableRefIndexMap>();
				}
				ref->emplace(indexReference, ref->size());
			}

		private:
			ValueVariant m_content;
			ValueType m_type = ValueType::EMPTY;
			InstanceReferences ref;
		};

		using Register = Value;
	}
}

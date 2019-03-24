#pragma once
#include <vector>
#include <variant>
#include <sstream>
#include <tuple>
#include <unordered_map>
#include <type_traits>
#include <memory>

#include "NodeValue/Token.h"

namespace ska {
	class NodeValue;
	class NodeCell;
	class MemoryTable;

	using NodeValueVariant_ = std::variant<
		Token::Variant,
		std::shared_ptr<std::vector<NodeValue>>,
		std::shared_ptr<std::unordered_map<std::string, NodeValue>>
	>;
	
	namespace detail {
		template<typename T, typename VARIANT_T>
		struct isVariantMember;

		template<typename T, typename... ALL_T>
		struct isVariantMember<T, std::variant<ALL_T...>>
			: public std::disjunction<std::is_same<T, ALL_T>...> {};
	}
	
	class NodeValue {
	public:
		NodeValue() = default;
		template <class Arg>
		NodeValue(Arg arg) : m_variant(std::move(arg)) {}
		NodeValue(std::shared_ptr<std::vector<NodeValue>> arg) : m_variant(arg) {}
		NodeValue(std::shared_ptr<std::unordered_map<std::string, NodeValue>> arg) : m_variant(arg) {}

		NodeValue(NodeValue&&) = default;

		NodeValue& operator=(NodeValue&&) = default;
		NodeValue& operator=(const NodeValue&) = delete;
		~NodeValue() = default;

		NodeValue clone() const { return *this; }

		template<class T> auto& as() { return std::get<T>(m_variant); }
		template<class T> const auto& as() const { return std::get<T>(m_variant); }
		
		double convertNumeric() const { 
			double numeric = 0.0;
			const auto& valueVariant = std::get<Token::Variant>(m_variant);
			std::visit([&numeric](auto && arg){
				using T = std::decay_t<decltype(arg)>;

				if constexpr (std::is_same<T, int>::value) {
					numeric = static_cast<double>(arg);
				} else if constexpr (std::is_same<T, double>::value) {
					numeric = arg;
				} else if constexpr (std::is_same<T, bool>::value) {
					numeric = arg ? 1.0 : 0.0;
				} else if constexpr (std::is_same<T, std::string>::value) {
					numeric = std::stod(arg);
				} else {
					throw std::runtime_error("cannot convert the node value to a numeric format");
				}
			}, valueVariant);
			return numeric;
		}

		std::string convertString() const {
			auto result = std::string{};
			const auto& valueVariant = std::get<Token::Variant>(m_variant);
			std::visit([&result](auto && arg) {
				using T = std::decay_t<decltype(arg)>;

				if constexpr (std::is_same<T, int>::value) {
					result = std::to_string(arg);
				} else if constexpr (std::is_same<T, double>::value) {
					result = std::to_string(arg);
				} else if constexpr (std::is_same<T, bool>::value) {
					result = arg ? "true" : "false";
				} else if constexpr (std::is_same<T, std::string>::value) {
					result = arg;
				} else {
					throw std::runtime_error("cannot convert the node value to a numeric format");
				}
			}, valueVariant);
			return result;
		}

		template <class Converted>
		Converted& nodeval() {
			if constexpr(detail::isVariantMember<Converted, Token::Variant>::value) {
				return std::get<Converted>(std::get<Token::Variant>(m_variant));
			} else {
				return std::get<Converted>(m_variant);
			}
		}

		template <class Converted>
		const Converted& nodeval() const {
			if constexpr(detail::isVariantMember<Converted, Token::Variant>::value) {
				return std::get<Converted>(std::get<Token::Variant>(m_variant));
			} else {
				return std::get<Converted>(m_variant);
			}
		}
		
		friend bool operator==(const NodeValue& lhs, const NodeValue& rhs);

	private:
		NodeValue(const NodeValue&) = default;
		NodeValueVariant_ m_variant;
	};


	class NodeCell {
	public:
		NodeCell() = default;
		NodeCell(MemoryTable& memory, NodeValue* lvalue): m_memory(&memory), m_variant(lvalue) {}
		NodeCell(NodeValue rvalue) : m_variant(std::move(rvalue)) {}
		NodeCell(std::pair<NodeValue*, MemoryTable*> lvalue) : m_memory(lvalue.second), m_variant(lvalue.first) { assert(m_memory != nullptr); }

		std::pair<NodeValue*, MemoryTable*> asLvalue() {
			assert(std::holds_alternative<NodeValue*>(m_variant) && "Must be an lvalue");
			return std::make_pair(std::get<NodeValue*>(m_variant), m_memory);
		}

		NodeValue asRvalue() {
			return std::holds_alternative<NodeValue*>(m_variant) ? std::move(*std::get<NodeValue*>(m_variant)) : std::move(std::get<NodeValue>(m_variant));
		}
		
		template<class T> auto& as() { return std::get<T>(m_variant); }
		template<class T> const auto& as() const { return std::get<T>(m_variant); }
		
		friend bool operator==(const NodeCell& lhs, const NodeCell& rhs);
	private:
		std::variant<NodeValue, NodeValue*> m_variant;
		MemoryTable* m_memory = nullptr;
	};

	inline bool operator==(const ska::NodeValue& lhs, const ska::NodeValue& rhs) {
		return lhs.m_variant == rhs.m_variant;
	}

	inline bool operator==(const ska::NodeCell& lhs, const ska::NodeCell& rhs) {
		return lhs.m_variant == rhs.m_variant;
	}
}


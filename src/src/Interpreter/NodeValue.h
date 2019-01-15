#pragma once
#include <vector>
#include <variant>
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
		template <class ... Args>
		NodeValue(Args&& ... args): m_variant(std::forward<Args>(args)...) {}

		NodeValue(NodeValue&&) = default;

		NodeValue& operator=(NodeValue&&) = default;
		NodeValue& operator=(const NodeValue&) = delete;
		~NodeValue() = default;

		NodeValue clone() const { return *this; }

		template<class T> auto& as() { return std::get<T>(m_variant); }
		template<class T> const auto& as() const { return std::get<T>(m_variant); }
		
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
		
	private:
		NodeValue(const NodeValue&) = default;
		NodeValueVariant_ m_variant;
	};


	class NodeCell {
	public:
		template <class ... Args>
		NodeCell(Args&& ... args): m_variant(std::forward<Args>(args)...) {}
		
		NodeValue& asLvalue() {
			assert(std::holds_alternative<NodeValue*>(m_variant));
			return *std::get<NodeValue*>(m_variant);
		}

		NodeValue asRvalue() {
			return std::holds_alternative<NodeValue*>(m_variant) ? std::move(*std::get<NodeValue*>(m_variant)) : std::move(std::get<NodeValue>(m_variant));
		}
		
		template<class T> auto& as() { return std::get<T>(m_variant); }
		template<class T> const auto& as() const { return std::get<T>(m_variant); }
		
	private:
		std::variant<NodeValue, NodeValue*> m_variant;
	};

}


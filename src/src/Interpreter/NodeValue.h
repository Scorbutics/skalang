#pragma once
#include <vector>
#include <unordered_map>
#include <type_traits>
#include <memory>

#include "NodeValue/Token.h"

namespace ska {
	class MemoryTable;
	
	class NodeValue;
	using NodeValueVariant_ = std::variant<
		Token::Variant,
		std::shared_ptr<std::vector<NodeValue>>,
		std::shared_ptr<std::unordered_map<std::string, NodeValue>>
	>;
	class NodeValue : public NodeValueVariant_ {
	public:
		using NodeValueVariant_::variant;

		NodeValue() = default;

		NodeValue(NodeValue&&) = default;

		NodeValue& operator=(NodeValue&&) = default;
		NodeValue& operator=(const NodeValue&) = delete;
		~NodeValue() = default;

		NodeValue clone() const {
			return *this;
		}

	private:
		NodeValue(const NodeValue&) = default;

	};

	class NodeCell : public std::variant<NodeValue, NodeValue*> {
	public:
		using std::variant<NodeValue, NodeValue*>::variant;
		NodeValue& asLvalue() {
			assert(std::holds_alternative<NodeValue*>(*this));
			return *std::get<NodeValue*>(*this);
		}

		NodeValue asRvalue() {
			return std::holds_alternative<NodeValue*>(*this) ? std::move(*std::get<NodeValue*>(*this)) : std::move(std::get<NodeValue>(*this));
		}
	};

	namespace detail {
		template<typename T, typename VARIANT_T>
		struct isVariantMember;

		template<typename T, typename... ALL_T>
		struct isVariantMember<T, std::variant<ALL_T...>>
			: public std::disjunction<std::is_same<T, ALL_T>...> {};
	}

	template <class Converted>
	Converted& nodeval(NodeValue& node) {
		if constexpr(detail::isVariantMember<Converted, Token::Variant>::value) {
			return std::get<Converted>(std::get<Token::Variant>(node));
		} else {
			return std::get<Converted>(node);
		}
	}

	template <class Converted>
	const Converted& nodeval(const NodeValue& node) {
		if constexpr (detail::isVariantMember<Converted, Token::Variant>::value) {
			return std::get<Converted>(std::get<Token::Variant>(node));
		} else {
			return std::get<Converted>(node);
		}
	}
}

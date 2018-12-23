#pragma once
#include <vector>
#include <unordered_map>
#include <type_traits>
#include <memory>

#include "NodeValue/Token.h"

namespace ska {
	class MemoryTable;
	using NodeValue = std::variant<
		Token::Variant, 
		std::vector<MemoryTable*>, 
		std::unordered_map<std::string, MemoryTable*>
	>;

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

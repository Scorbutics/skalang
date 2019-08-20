#pragma once
#include <vector>
#include <variant>
#include <sstream>
#include <tuple>
#include <unordered_map>
#include <type_traits>
#include <memory>

#include "TokenVariant.h"
#include "NodeValueArray.h"
#include "NodeValueMap.h"

namespace ska {
	namespace bytecode {
		class ExecutionContext;

		using NodeValueVariant_ = std::variant<
			TokenVariant,
			NodeValueArray,
			NodeValueMap
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
			NodeValue(Arg&& arg) : m_variant(std::forward<Arg>(arg)) {}
			NodeValue(NodeValueArray arg) : m_variant(arg) {}
			NodeValue(NodeValueMap arg) : m_variant(arg) {}

			NodeValue(NodeValue&&) noexcept = default;

			NodeValue& operator=(NodeValue&&) noexcept = default;
			NodeValue& operator=(const NodeValue&) = delete;
			~NodeValue() = default;

			NodeValue clone() const { return *this; }

			template<class T> auto& as() { return std::get<T>(m_variant); }
			template<class T> const auto& as() const { return std::get<T>(m_variant); }

			double convertNumeric() const;

			std::string convertString() const;

			template <class Converted>
			static constexpr bool is_container_of_values() {
				return !detail::isVariantMember<Converted, TokenVariant>::value;
			}

			template <class Converted>
			Converted& nodeval() {
				if constexpr(detail::isVariantMember<Converted, TokenVariant>::value) {
					return std::get<Converted>(std::get<TokenVariant>(m_variant));
				} else {
					return std::get<Converted>(m_variant);
				}
			}

			template <class Converted>
			const Converted& nodeval() const {
				if constexpr(detail::isVariantMember<Converted, TokenVariant>::value) {
					return std::get<Converted>(std::get<TokenVariant>(m_variant));
				} else {
					return std::get<Converted>(m_variant);
				}
			}

			friend bool operator==(const NodeValue& lhs, const NodeValue& rhs);

		private:
			NodeValue(const NodeValue&) = default;
			NodeValueVariant_ m_variant;
		};

		bool operator==(const NodeValue& lhs, const NodeValue& rhs);
	}
}

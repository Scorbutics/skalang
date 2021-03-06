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
#include "NodeValueFunctionMember.h"

namespace ska {
	class NodeValue;

	using NodeValueVariant_ = std::variant<
		TokenVariant,
		NodeValueFunctionMember,
		NodeValueArray,
		NodeValueMap,
		NodeValue*
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
		NodeValue():
			m_emptyVariant(true) {
		}

		template <class Arg>
		NodeValue(Arg&& arg) {
			if constexpr(std::is_same_v<NodeValue, std::remove_const_t<std::remove_reference_t<Arg>>>) {
				*this = std::forward<Arg>(arg);
			} else {
				transferValueToOwned(std::forward<Arg>(arg));
			}
		}

		NodeValue(NodeValueArray arg) :
			m_variant(arg),
			m_emptyVariant(false) {
		}

		NodeValue(NodeValueMap arg) :
			m_variant(arg),
			m_emptyVariant(false) {
		}

		NodeValue(NodeValue&&) noexcept = default;

		NodeValue& operator=(NodeValue&& arg) noexcept {
			transferValueToOwned(std::move(arg.m_variant));
			m_emptyVariant = arg.m_emptyVariant;
			return *this;
		}

		NodeValue& operator=(const NodeValue& arg) {
			transferValueToOwned(arg.m_variant);
			m_emptyVariant = arg.m_emptyVariant;
			return *this;
		}
		
		void release();

		NodeValue(const NodeValue&) = default;
		~NodeValue() = default;

		template<class T>
		T& as() {
			if(std::holds_alternative<NodeValue*>(m_variant)) {
				return std::get<NodeValue*>(m_variant)->as <T>();
			}
			return std::get<T>(m_variant);
		}

		template<class T>
		const T& as() const {
			if(std::holds_alternative<NodeValue*>(m_variant)) {
				return std::get<NodeValue*>(m_variant)->as <T>();
			}
			return std::get<T>(m_variant);
		}

		double convertNumeric() const;
		bool empty() const { return m_emptyVariant;	}
		std::string convertString() const;

		template <class Converted>
		static constexpr bool is_container_of_values() {
			return !detail::isVariantMember<Converted, TokenVariant>::value;
		}

		template <class Converted>
		Converted& nodeval() {
			return const_cast<Converted&>(static_cast<const NodeValue&>(*this).nodeval<Converted>());
		}

		template <class Converted>
		const Converted& nodeval() const {
			if(std::holds_alternative<NodeValue*>(m_variant)) {
				return std::get<NodeValue*>(m_variant)->nodeval <Converted>();
			}
			if constexpr(detail::isVariantMember<Converted, TokenVariant>::value) {
				return std::get<Converted>(std::get<TokenVariant>(m_variant));
			} else {
				return std::get<Converted>(m_variant);
			}
		}

		friend bool operator==(const NodeValue& lhs, const NodeValue& rhs);

	private:
		void transferValueToOwned(NodeValueVariant_ arg);
		static NodeValue& dereference(const NodeValueVariant_& arg);
		static bool isReference(const NodeValueVariant_& arg);

		NodeValueVariant_ m_variant;
		bool m_emptyVariant = false;
		bool m_dirty = false;
	};

	bool operator==(const NodeValue& lhs, const NodeValue& rhs);
}

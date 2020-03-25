#pragma once
#include <unordered_map>
#include <vector>
#include <sstream>
#include "Config/LoggerConfigLang.h"
#include "ExpressionType.h"
#include "SymbolTypeAccess.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::Type)

namespace ska {
	class Symbol;
	class SymbolTable;
	class TypeCrosser;

	struct Type {
		static constexpr bool isNamed(ExpressionType type) {
			return type == ExpressionType::FUNCTION || type == ExpressionType::OBJECT;
		}

		static constexpr bool isNumeric(ExpressionType type) {
			return type == ExpressionType::INT || type == ExpressionType::FLOAT || type == ExpressionType::BOOLEAN;
		}

		static constexpr bool isBuiltIn(ExpressionType type) {
			return isNumeric(type) || type == ExpressionType::STRING || type == ExpressionType::VOID;
		}

		static bool isNamed(const Type& type) {
			return isNamed(type.m_type);
		}

		static bool isNumeric(const Type& type) {
			return isNumeric(type.m_type);
		}

		static bool isBuiltIn(const Type& type) {
			return isBuiltIn(type.m_type);
		}

		static Type MakeBuiltInArray(ExpressionType t) {
			assert(t != ExpressionType::VOID);
			auto result = Type{ ExpressionType::ARRAY };
			result.m_compound.push_back(Type{ t });
			return result;
		}

		template<ExpressionType t>
		static Type MakeBuiltIn() {
			static_assert(t != ExpressionType::VOID);
			return Type{ t };
		}

		static Type MakeBuiltIn(ExpressionType t, const Symbol* symbol = nullptr) {
			return Type{ symbol, t };
		}

		template<ExpressionType t>
		static Type MakeCustom(const Symbol* symbol) {
			static_assert(isNamed(t));
			return Type{ symbol, t };
		}

		static Type Override(Type t, const Symbol* symbol) {
			t.m_symbol = symbol;
			return t;
		}


		Type() = default;
		Type(Type&& t) noexcept = default;
		Type(const Type& t) = default;
    	
		Type& operator=(Type&& t) noexcept = default;
		Type& operator=(const Type& t) = default;
	
		~Type() = default;

		ExpressionType type() const {
			return m_type;
		}
		bool operator==(const Type& t) const;


		bool operator==(const ExpressionType& t) const {
			return m_type == t;
		}

		bool operator!=(const ExpressionType& t) const {
			return m_type != t;
		}

		Type& operator[](std::size_t index) { return m_compound[index]; }
		const Type& operator[](std::size_t index) const { return m_compound[index]; }

		Type& operator=(ExpressionType t) {
			m_type = std::move(t);
			return *this;
		}

		Type& add(Type t) {
			m_compound.push_back(std::move(t));
			return *this;
		}

		bool operator!=(const Type& t) const {
			return !(*this == t);
		}

		bool structuralEquality(const Type&) const;

		Type crossTypes(const TypeCrosser& crosser, std::string op, const Type& type2) const;
		
		std::size_t size() const { return m_compound.size(); }
		bool empty() const { return m_compound.empty(); }

		const Type& back() const { return m_compound.back(); }

		std::string name() const;
		
		auto begin() const { return m_compound.begin(); }
		auto end() const { return m_compound.end(); }
		auto begin() { return m_compound.begin(); }
		auto end() { return m_compound.end(); }

		//bool hasSymbol(const Type* otherType) const {
		//	return otherType == nullptr ? m_symbol == nullptr : m_symbol == otherType->m_symbol;
		//}

		bool tryChangeSymbol(const Type& type);

	private:
		friend class TypeCrosser;

		explicit Type(ExpressionType t) :
			m_type(std::move(t)) {
		}

		Type(const Symbol* symbol, ExpressionType t);

		ExpressionType m_type = ExpressionType::VOID;
    	const Symbol* m_symbol = nullptr;
		std::vector<Type> m_compound;

		friend std::ostream& operator<<(std::ostream& stream, const Type& type);
	};
	
	std::ostream& operator<<(std::ostream& stream, const Type& type);
}

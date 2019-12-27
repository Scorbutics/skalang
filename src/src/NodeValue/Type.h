#pragma once
#include <unordered_map>
#include <vector>
#include <sstream>
#include "Config/LoggerConfigLang.h"
#include "ExpressionType.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::Type)

namespace ska {
	class Symbol;
	class SymbolTable;
	class TypeCrosser;

	struct Type {
		friend class Symbol;
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

		bool hasSymbol() const {
			return m_symbol != nullptr;
		}

		bool operator==(const ExpressionType& t) const {
			return m_type == t;
		}

		bool operator!=(const ExpressionType& t) const {
			return m_type != t;
		}

		Type& operator=(ExpressionType t) {
			m_type = std::move(t);
			return *this;
		}

		Type& add(Type t) {
			m_compound.push_back(std::move(t));
			return *this;
		}

		const std::vector<Type>& compound() const {
			return m_compound;
		}

		bool operator!=(const Type& t) const {
			return !(*this == t);
		}

		Type crossTypes(const TypeCrosser& crosser, std::string op, const Type& type2) const;
		
		std::size_t size() const {
			return m_compound.size();
		}

		const Symbol* operator[](const std::string& fieldName) const;
		const Symbol* symbol() const { return m_symbol; }

	private:
		friend class TypeCrosser;
		bool equalIgnoreSymbol(const Type& t) const;
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

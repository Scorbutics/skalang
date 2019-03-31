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

	struct Type {
		static constexpr bool isNamed(ExpressionType type) {
			return type == ExpressionType::FUNCTION || type == ExpressionType::OBJECT;
		}

		static constexpr bool isNumeric(ExpressionType type) {
			return type == ExpressionType::INT || type == ExpressionType::FLOAT || type == ExpressionType::BOOLEAN;
		}

		static bool isNamed(Type type) {
			return isNamed(type.m_type);
		}

		static bool isNumeric(Type type) {
			return isNumeric(type.m_type);
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

		template<ExpressionType t>
		static Type MakeCustom(const std::string symbol) {
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

		Type updateSymbol(const SymbolTable& symbols) const;

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

		ExpressionType crossTypes(std::string op, const Type& type2) const;
		
		std::size_t size() const {
			return m_compound.size();
		}

		const Symbol* operator[](const std::string& fieldName) const;

	private:
	    static constexpr auto TypeMapSize = static_cast<std::size_t>(ExpressionType::UNUSED_Last_Length);
        static const std::unordered_map<std::string, int(*)[TypeMapSize][TypeMapSize]>& GetMap(const std::string& op);

		explicit Type(ExpressionType t) :
			m_type(std::move(t)) {
		}

		Type(const Symbol* symbol, ExpressionType t);

		Type(std::string symbol, ExpressionType t) :
			m_type(t),
			m_symbolAlias(std::move(symbol)) {
		}

		bool checkSameObjectTypes(const Type& type2) const;

		ExpressionType m_type = ExpressionType::VOID;
        const Symbol* m_symbol = nullptr;
		std::string m_symbolAlias;
		std::vector<Type> m_compound;

		friend std::ostream& operator<<(std::ostream& stream, const Type& type);
    };
	
	std::ostream& operator<<(std::ostream& stream, const Type& type);
}

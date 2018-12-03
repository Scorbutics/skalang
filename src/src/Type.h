#pragma once
#include <vector>
#include <sstream>
#include "LoggerConfigLang.h"
#include "ExpressionType.h"

SKA_LOGC_CONFIG(ska::LogLevel::Info, ska::Type)

namespace ska {

	struct Type {
		Type() = default;
		
		static constexpr bool isNamed(ExpressionType type) {
			return type == ExpressionType::FUNCTION || type == ExpressionType::OBJECT;
		}

		static bool isNamed(Type type) {
			return isNamed(type.m_type);
		}

		explicit Type(ExpressionType t) : m_type(std::move(t)) {
			//assert(!isNamed(m_type));
		}

		Type(Type t, const ScopedSymbolTable& sym) {
			*this = t;
			m_usedDefinedSymbolTable = &sym;
			assert(isNamed(m_type));
		}

		Type(std::string name, ExpressionType t, const ScopedSymbolTable& sym) : m_type(t), m_usedDefinedSymbolTable(&sym), m_alias(std::move(name)) {
			assert(isNamed(m_type));
		}

        Type(Type&& t) noexcept {
            *this = std::move(t);
        }

        Type& operator=(Type&& t) noexcept {
			SLOG(ska::LogLevel::Debug) << "   Move, Type " << t;
            m_type = std::move(t.m_type);
            m_alias = std::move(t.m_alias);
            m_compound = std::move(t.m_compound);
			m_usedDefinedSymbolTable = std::move(t.m_usedDefinedSymbolTable);
			t.m_usedDefinedSymbolTable = nullptr;
            t.m_moved = true;
			SLOG(ska::LogLevel::Debug) << " moved  to " << *this;
            return *this;
        }

        ~Type() = default;

		Type(const Type& t) {
			*this = t;
		}

		Type& operator=(const Type& t) {
			m_type = t.m_type;
			m_alias = t.m_alias;
			m_compound = t.m_compound;
			m_usedDefinedSymbolTable = t.m_usedDefinedSymbolTable;
			m_moved = t.m_moved;
			SLOG(ska::LogLevel::Debug) << "   Copy, Type " << t << " copied to " << *this;
			return *this;
		}
	
		bool operator==(const Type& t) const {
			if (!m_alias.empty()) {
				return m_alias == t.m_alias;
			}

			if (!t.m_alias.empty()) {
				return false;
			}

			return m_type == t.m_type && m_compound == t.m_compound;
		}

        const std::string& getName() const {
            return m_alias;
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

		ExpressionType crossTypes(char op, const Type& type2) const;
		
		const ScopedSymbolTable* userDefinedSymbolTable() const {
			return m_usedDefinedSymbolTable;
		}

		std::size_t size() const {
			return m_compound.size();
		}

	private:
		ExpressionType m_type = ExpressionType::VOID;
		std::string m_alias;
		std::vector<Type> m_compound;
		const ScopedSymbolTable* m_usedDefinedSymbolTable = nullptr;
	    bool m_moved = false;

		friend std::ostream& operator<<(std::ostream& stream, const Type& type);
    };
	
	inline std::ostream& operator<<(std::ostream& stream, const Type& type) {
		if (type.m_moved) {
			stream << "INVALID_MOVED";
			return stream;
		} 

		if (type.m_compound.empty()) {
			stream << type.m_alias + " " + ExpressionTypeSTR[static_cast<std::size_t>(type.m_type)];

		} else {
			stream << type.m_alias << " (" << ExpressionTypeSTR[static_cast<std::size_t>(type.m_type)];
			for (const auto& childType : type.m_compound) {
				stream << " - " << childType;
			}
			stream << ")";
		}
		
		return stream;
	}
}
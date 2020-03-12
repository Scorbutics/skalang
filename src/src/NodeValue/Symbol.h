#pragma once
#include <optional>
#include <unordered_set>
#include "Type.h"
#include "Service/SymbolFieldResolver.h"

namespace ska {
	class Symbol;
}

namespace ska {

	class Symbol {
		friend class ScopedSymbolTable;
	private:
		Symbol() = default;

		Symbol(std::size_t tableIndex, std::string name, SymbolFieldResolver fields) :
			m_name(std::move(name)),
			m_data(std::move(fields)),
			m_tableIndex(tableIndex) {
		}

	public:
		Symbol(const Symbol& s) {
			*this = s;
		}

		Symbol(Symbol&& s) noexcept {
			*this = std::move(s);
		}

		Symbol& operator=(const Symbol& s);
		Symbol& operator=(Symbol&& s) noexcept;

		const std::string& name() const { return m_name; }
		ExpressionType nativeType() const { return m_category.type(); }
		const Type& type() const { return m_category; }
		bool changeTypeIfRequired(const Type& type);

		std::size_t size() const;
		bool empty() const { return m_category.compound().empty(); }

		void implement(Symbol& symbol);
		const Symbol* master() const { return m_master; }

		const Symbol* operator[](const std::string& fieldSymbolName) const;
		bool operator==(const Symbol& sym) const;
		bool operator!=(const Symbol& sym) const {	return !(*this == sym);	}

	private:
		SymbolFieldResolver m_data = SymbolFieldResolver{static_cast<ScopedSymbolTable*>(nullptr)};
		std::string m_name;
		Type m_category;
		std::unordered_set<Symbol*> m_implementationReferences;
		Symbol* m_master = this;
		std::size_t m_tableIndex = 0;
	};
}
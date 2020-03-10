#pragma once
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

		Symbol(std::string name, SymbolFieldResolver fields) :
			m_name(std::move(name)),
			m_data(std::move(fields)) {
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

		void forceType(Type t);

		const std::string& name() const { return m_name; }
		const Type& type() const { return m_category; }

		std::size_t size() const;
		bool empty() const { return m_category.compound().empty(); }

		const Type& operator()(std::size_t index) const { return m_category.compound()[index]; }
		const Symbol* operator[](const std::string& fieldSymbolName) const;
		bool operator==(const Symbol& sym) const;
		bool operator!=(const Symbol& sym) const {	return !(*this == sym);	}

	private:
		SymbolFieldResolver m_data = SymbolFieldResolver{static_cast<ScopedSymbolTable*>(nullptr)};
		std::string m_name;
		Type m_category;
	};
}
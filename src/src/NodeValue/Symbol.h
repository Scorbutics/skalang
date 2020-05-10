#pragma once
#include <optional>
#include <unordered_set>
#include "Type.h"
#include "Service/SymbolFieldResolver.h"

namespace ska {
	class Symbol;
	class ScopedSymbolTable;
	struct ScriptHandleAST;
	class SymbolFactory;
}

namespace ska {

	class Symbol {
	private:
		friend class SymbolFactory;
		Symbol() = default;

		Symbol(std::size_t tableIndex, std::string name, SymbolFieldResolver fields);

	public:
		Symbol(const Symbol& s) = delete;
		Symbol(Symbol&& s) noexcept;

		Symbol& operator=(const Symbol& s) = delete;
		Symbol& operator=(Symbol&& s) noexcept;

		const std::string& name() const { return m_name; }
		ExpressionType nativeType() const { return m_category.type(); }
		const Type& type() const { return m_category; }
		bool changeTypeIfRequired(const Type& type);

		void openTable();
		void closeTable();

		std::size_t size() const;
		bool empty() const;

		void implement(Symbol& symbol);
		const Symbol* master() const { return m_master; }

		std::size_t id(const Symbol& field) const;

		const Symbol* back() const;
		Symbol* back();

		auto begin() const { return m_data.begin(); }
		auto begin() { return m_data.begin(); }
		auto end() const { return m_data.end(); }
		auto end() { return m_data.end(); }

		const Symbol* operator[](std::size_t index) const;
		Symbol* operator[](std::size_t index);

		const Symbol* operator()(const std::string& fieldSymbolName) const;
		Symbol* operator()(const std::string& fieldSymbolName);

		bool operator==(const Symbol& sym) const;
		bool operator!=(const Symbol& sym) const {	return !(*this == sym);	}

	private:
		friend std::ostream& operator<<(std::ostream& stream, const Symbol& symbol);
		std::string m_name;
		std::size_t m_tableIndex = 0;
		SymbolFieldResolver m_data = SymbolFieldResolver{m_name, m_tableIndex, static_cast<ScopedSymbolTable*>(nullptr)};
		Type m_category;
		std::unordered_set<Symbol*> m_implementationReferences;
		Symbol* m_master = this;
		bool m_closed = true;
		
	};
	std::ostream& operator<<(std::ostream& stream, const Symbol& symbol);
}

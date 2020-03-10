#pragma once
#include "Type.h"
#include "Service/SymbolFieldResolver.h"

namespace ska {
	class Symbol;
}

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::Symbol)

namespace ska {

	class Symbol {
		friend class ScopedSymbolTable;
	private:
		Symbol() {}

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

		Symbol& operator=(const Symbol& s) {
			m_data = s.m_data;
			m_name = s.m_name;
			m_category = s.m_category;
			SLOG(ska::LogLevel::Debug) << "   Copy, Symbol " << s.getName() << " " << s.m_category << " copied to " << m_name << " " << m_category;
			return *this;
		}

		Symbol& operator=(Symbol&& s) noexcept {
			m_data = std::move(s.m_data);
			m_name = std::move(s.m_name);
			m_category = std::move(s.m_category);
			SLOG(ska::LogLevel::Debug) << "   Move, Symbol " << s.getName() << " " << s.m_category << " moved to " << m_name << " " << m_category;
			return *this;
		}

		const Type& operator()(std::size_t index) const {
			return m_category.compound()[index];
		}

		Type operator()(std::size_t index) {
			return m_category.compound()[index];
		}

		const std::string& getName() const {
			return m_name;
		}

		const Type& getType() const {
			return m_category;
		}

		void forceType(Type t);

		bool empty() const {
			return m_category.compound().empty();
		}

		const Symbol* operator[](const std::string& fieldSymbolName) const;
		Symbol* operator[](const std::string& fieldSymbolName);

		std::size_t size() const;

		bool operator==(const Symbol& sym) const;

		bool operator!=(const Symbol& sym) const {
			return !(*this == sym);
		}

	private:
		SymbolFieldResolver m_data = SymbolFieldResolver{static_cast<ScopedSymbolTable*>(nullptr)};
		std::string m_name;
		Type m_category;
	};
}
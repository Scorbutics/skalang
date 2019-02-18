#pragma once
#include "Type.h"

namespace ska {
	class Symbol;
}

SKA_LOGC_CONFIG(ska::LogLevel::Info, ska::Symbol)

namespace ska {
	class ScopedSymbolTable;
 	
    class Symbol {
        friend class ScopedSymbolTable;
        private:
			Symbol(){}

			Symbol(std::string name, ScopedSymbolTable& symbolTable, Type type = Type {}) :
				m_name(std::move(name)), 
				m_category(std::move(type)),
				m_scopedTable(&symbolTable) {
				SLOG(ska::LogLevel::Debug) << "Creating Symbol \"" << m_name << "\" with type " << m_category;
            }
        public:
            Symbol(const Symbol& s) {
                *this = s;
            }

            Symbol(Symbol&& s) noexcept {
                *this = std::move(s);
            }

            Symbol& operator=(const Symbol& s) {
                m_scopedTable = s.m_scopedTable;
                m_name = s.m_name;
                m_category = s.m_category;
				SLOG(ska::LogLevel::Debug) << "   Copy, Symbol " << s.getName() << " " << s.m_category << " copied to " << m_name << " " << m_category;
                return *this;
            }

            Symbol& operator=(Symbol&& s) noexcept {
                m_scopedTable = std::move(s.m_scopedTable);
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

			void forceType(Type t) {
				m_category = t;
			}

            bool empty() const {
                return m_category.compound().empty();
            }

            const Symbol* operator[](const std::string& fieldSymbolName) const;
            Symbol* operator[](const std::string& fieldSymbolName);

            std::size_t size() const;

        private:
			ScopedSymbolTable* m_scopedTable = nullptr;
            std::string m_name;
            Type m_category;
    };
}
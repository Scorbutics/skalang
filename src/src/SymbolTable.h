#pragma once
#include <Utils/SubObserver.h>
#include <Utils/Observable.h>

#include "AST.h"
#include "VarTokenEvent.h"
#include "BlockTokenEvent.h"
#include "FunctionTokenEvent.h"
#include "ReturnTokenEvent.h"
#include "ExpressionType.h"

namespace ska {
	class Symbol;
}

SKA_LOGC_CONFIG(ska::LogLevel::Info, ska::Symbol)

namespace ska {
    class ScopedSymbolTable;
    	
    class Symbol {
        public:
            Symbol(std::string name, Type cat, ScopedSymbolTable& symbolTable) : 
				m_name(std::move(name)), 
                m_category(std::move(cat)),
				m_scopedTable(&symbolTable) {
					SLOG(ska::LogLevel::Debug) << "Creating Symbol " << m_name << " with type " << m_category.asString();
                }

            Symbol(const Symbol& s) {
                *this = s;
            }

            Symbol(Symbol&& s) {
                *this = std::move(s);
            }

            Symbol& operator=(const Symbol& s) {
                m_scopedTable = s.m_scopedTable;
                m_name = s.m_name;
                m_category = s.m_category;
				SLOG(ska::LogLevel::Debug) << "   Copy, Symbol " << s.getName() << " " << s.m_category.asString() << " copied to " << m_name << " " << m_category.asString();
                return *this;
            }

            Symbol& operator=(Symbol&& s) {
                m_scopedTable = std::move(s.m_scopedTable);
                m_name = std::move(s.m_name);
                m_category = std::move(s.m_category);
				SLOG(ska::LogLevel::Debug) << "   Move, Symbol " << s.getName() << " " << s.m_category.asString() << " moved to " << m_name << " " << m_category.asString();
                return *this;
            }

            const Type& operator[](std::size_t index) const {
                return m_category.compound()[index];
            }

			Type operator[](std::size_t index) {
				return m_category.compound()[index];
			}

            const std::string& getName() const {
                return m_name;
            }

            const Type& getType() const {
                return m_category;
            }

			void setType(Type t) {
				m_category = std::move(t);
			}

			ScopedSymbolTable* symbolTable() {
				return m_scopedTable;
			}

            bool empty() const {
                return m_category.compound().empty();
            }

            const Symbol* operator[](const std::string& symbol) const;
            Symbol* operator[](const std::string& symbol);

            std::size_t size() const;

            void link(std::vector<Symbol> subtypes, ScopedSymbolTable& table);

        private:
			ScopedSymbolTable* m_scopedTable = nullptr;
            std::string m_name;
            Type m_category;
    };

	class SymbolTable;

	class ScopedSymbolTable {
	using ChildrenScopedSymbolTable = std::vector<std::unique_ptr<ScopedSymbolTable>>;
	
	friend class SymbolTable;
	public:
		ScopedSymbolTable(ScopedSymbolTable& parent) :
			m_parent(parent) {
		}

		ScopedSymbolTable() = default;
		~ScopedSymbolTable() = default;

		ScopedSymbolTable& parent();
		ScopedSymbolTable& createNested();
		Symbol& emplace(std::string name, Type type);
		
        void link(Symbol& s) {
            m_parentSymbol = &s;
        }

        Symbol* parentSymbol() {
            return m_parentSymbol;
        }

		Symbol* operator[](const std::string& key) {
            auto valueIt = m_symbols.find(key);
			if(valueIt == m_symbols.end()) {
				return &m_parent == this ? nullptr : m_parent[key];
			}
			return &m_symbols.at(key);
        }

        const Symbol* operator[](const std::string& key) const {
            const auto valueIt = m_symbols.find(key);
            if(valueIt == m_symbols.end()) {
                return &m_parent == this ? nullptr : m_parent[key];
            }
            return &m_symbols.at(key);
        }
		
		ChildrenScopedSymbolTable& children() {
			return m_children;
		}
		
		const ChildrenScopedSymbolTable& children() const {
			return m_children;
		}

	private:
        std::unordered_map<std::string, Symbol> m_symbols;
		ChildrenScopedSymbolTable m_children;
		ScopedSymbolTable& m_parent = *this;
        Symbol* m_parentSymbol = nullptr;
	};

    class Parser;

	class SymbolTable :
        public SubObserver<VarTokenEvent>,
       	public SubObserver<BlockTokenEvent>,
        public SubObserver<FunctionTokenEvent>,
        public SubObserver<ReturnTokenEvent> {

        using ASTNodePtr = std::unique_ptr<ska::ASTNode>;

    public:
		SymbolTable(Parser& parser);
		~SymbolTable() = default;
		
		auto* operator[](const std::string& key) {
            return (*m_currentTable)[key];
        }

        const auto* operator[](const std::string& key) const {
            return (*m_currentTable)[key];
        }

		ScopedSymbolTable::ChildrenScopedSymbolTable& nested() {
			return m_currentTable->children();
		}

		const ScopedSymbolTable::ChildrenScopedSymbolTable& nested() const {
			return m_currentTable->children();
		}

		ScopedSymbolTable* current() {
			return m_currentTable;
		}

		const ScopedSymbolTable* current() const {
			return m_currentTable;
		}

    private:
		bool match(const VarTokenEvent&);
		bool nestedTable(const BlockTokenEvent&);
		bool matchFunction(const FunctionTokenEvent&);
		bool matchReturn(const ReturnTokenEvent&);

        std::unique_ptr<ScopedSymbolTable> m_rootTable;
		ScopedSymbolTable* m_currentTable = nullptr;
    };
}

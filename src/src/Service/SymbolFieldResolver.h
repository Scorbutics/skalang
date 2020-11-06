#pragma once
#include <variant>
#include <mutex>
#include <memory>

namespace ska {
	class ScopedSymbolTable;
	struct ScriptHandleAST;
    class Symbol;
    class SymbolTable;

    class SymbolFieldResolver {
    public:
        using Iterator = std::vector<std::unique_ptr<Symbol>>::iterator;
        using ConstIterator = std::vector<std::unique_ptr<Symbol>>::const_iterator;
        using Variant = std::variant<ScopedSymbolTable*, ScriptHandleAST*>;

        SymbolFieldResolver(const std::string& symbolName, std::size_t tableIndex, Variant value);

        SymbolFieldResolver(SymbolFieldResolver&& toMove);
        SymbolFieldResolver& operator=(SymbolFieldResolver&& toMove);
        SymbolFieldResolver(const SymbolFieldResolver& toCopy);
        SymbolFieldResolver& operator=(const SymbolFieldResolver& toCopy);

        ScopedSymbolTable* lookup();
        const ScopedSymbolTable* lookup() const;

        void open() { m_closed = false; }
        void close() { m_closed = true; }

        Iterator begin();
        Iterator end();
        ConstIterator begin() const;
        ConstIterator end() const;

        friend bool operator==(const SymbolFieldResolver& lhs, const SymbolFieldResolver& rhs);
        friend bool operator!=(const SymbolFieldResolver& lhs, const SymbolFieldResolver& rhs);
    private:
        std::vector<std::unique_ptr<Symbol>> m_defaultEmptyVector;
        std::size_t m_tableIndex;
        std::string m_symbolName;
        Variant m_inputData;
        bool m_closed = true;
        mutable ScopedSymbolTable* m_cacheData = nullptr;
        mutable std::mutex m_cacheMutex {};
    };

  bool operator==(const SymbolFieldResolver& lhs, const SymbolFieldResolver& rhs);
  bool operator!=(const SymbolFieldResolver& lhs, const SymbolFieldResolver& rhs);
}

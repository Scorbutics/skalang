#pragma once
#include <variant>

namespace ska {
	class ScopedSymbolTable;
	struct ScriptHandleAST;
    class Symbol;

	class SymbolFieldResolver {
  public:
    using Variant = std::variant<ScopedSymbolTable*, ScriptHandleAST*>;
    SymbolFieldResolver(Variant value);

    Symbol* lookup(std::size_t tableIndex, const std::string& fieldName);
    const Symbol* lookup(std::size_t tableIndex, const std::string& fieldName) const;

    friend bool operator==(const SymbolFieldResolver& lhs, const SymbolFieldResolver& rhs);
    friend bool operator!=(const SymbolFieldResolver& lhs, const SymbolFieldResolver& rhs);
  private:
    Variant m_data;
  };

  bool operator==(const SymbolFieldResolver& lhs, const SymbolFieldResolver& rhs);
  bool operator!=(const SymbolFieldResolver& lhs, const SymbolFieldResolver& rhs);
}

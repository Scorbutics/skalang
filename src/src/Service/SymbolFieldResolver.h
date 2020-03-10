#pragma once
#include <variant>

namespace ska {
	class ScopedSymbolTable;
	struct ScriptHandleAST;
  class Symbol;

	class SymbolFieldResolver {
  public:
    using Variant = std::variant<ScopedSymbolTable*, const ScriptHandleAST*>;
    SymbolFieldResolver(Variant value);
    const Symbol* operator[](const std::string& fieldName) const;

    friend bool operator==(const SymbolFieldResolver& lhs, const SymbolFieldResolver& rhs);
    friend bool operator!=(const SymbolFieldResolver& lhs, const SymbolFieldResolver& rhs);
  private:
    Variant m_data;
  };

  bool operator==(const SymbolFieldResolver& lhs, const SymbolFieldResolver& rhs);
  bool operator!=(const SymbolFieldResolver& lhs, const SymbolFieldResolver& rhs);
}

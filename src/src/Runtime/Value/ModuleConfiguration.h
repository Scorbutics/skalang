#pragma once
#include "NodeValue/ScriptCacheAST.h"
#include "Runtime/Value/InterpreterTypes.h"

namespace ska {
  class TypeBuilder;
  class SymbolTableUpdater;
  struct ReservedKeywordsPool;
  class StatementParser;
  class Interpreter;

  namespace lang {

    template <class Interpreter>
    struct ModuleConfiguration {
        ScriptCacheAST& scriptAstCache;
        typename InterpreterTypes<Interpreter>::ScriptCache& scriptCache;
        TypeBuilder& typeBuilder;
        SymbolTableUpdater& symbolTableUpdater;
        const ReservedKeywordsPool& reservedKeywords;
        StatementParser& parser;
        Interpreter& interpreter;
    };
  }
}
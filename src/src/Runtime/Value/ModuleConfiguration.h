#pragma once
#include "NodeValue/ScriptCacheAST.h"
#include "Runtime/Value/InterpreterTypes.h"

namespace ska {
  class TypeBuilder;
  class SymbolTableUpdater;
  class SemanticTypeChecker;
  struct ReservedKeywordsPool;
  class StatementParser;
  class Interpreter;

  namespace lang {

      struct BaseModuleConfiguration {
          ScriptCacheAST& scriptAstCache;
          TypeBuilder& typeBuilder;
          SymbolTableUpdater& symbolTableUpdater;
          SemanticTypeChecker& typeChecker;
          const ReservedKeywordsPool& reservedKeywords;
          StatementParser& parser;
      };

    template <class Interpreter>
    struct ModuleConfiguration : public BaseModuleConfiguration {
        ModuleConfiguration(ScriptCacheAST& scriptAstCache,
            TypeBuilder& typeBuilder,
            SymbolTableUpdater& symbolTableUpdater,
            SemanticTypeChecker& typeChecker,
            const ReservedKeywordsPool& reservedKeywords,
            StatementParser& parser,
            typename InterpreterTypes<Interpreter>::ScriptCache& scriptCache,
            Interpreter& interpreter) : 
            BaseModuleConfiguration{scriptAstCache, typeBuilder, symbolTableUpdater, typeChecker, reservedKeywords, parser},
            scriptCache(scriptCache),
            interpreter(interpreter){}

        typename InterpreterTypes<Interpreter>::ScriptCache& scriptCache;
        Interpreter& interpreter;
    };
  }
}
#pragma once

#include "Interpreter/ScriptCache.h"

namespace ska {
    class TypeBuilder;
    class SymbolTableTypeUpdater;
    class ReservedKeywordsPool;
    class StatementParser;
    class Interpreter;

    namespace lang {
        struct ModuleConfiguration {
            ScriptCache& scriptCache;
            TypeBuilder& typeBuilder;
            SymbolTableTypeUpdater& symbolTableTypeUpdater;
            const ReservedKeywordsPool& reservedKeywords;
            StatementParser& parser;
            Interpreter& interpreter;
        }; 
    }
}

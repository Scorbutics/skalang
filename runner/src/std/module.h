#pragma once

#include "Interpreter/ScriptCache.h"

namespace ska {
    class TypeBuilder;
    class SymbolTableTypeUpdater;
    
    class ReservedKeywordsPool;

    namespace lang {
        struct ModuleConfiguration {
            ScriptCache& scriptCache;
            TypeBuilder& typeBuilder;
            SymbolTableTypeUpdater& symbolTableTypeUpdater;
            const ReservedKeywordsPool& reservedKeywords;
        }; 
    }
}

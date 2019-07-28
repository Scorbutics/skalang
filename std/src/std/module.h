#pragma once

#include "Interpreter/ScriptCache.h"
#include "Interpreter/ScriptBinding.h"

namespace ska {
    class TypeBuilder;
    class SymbolTableUpdater;
    struct ReservedKeywordsPool;
    class StatementParser;
    class Interpreter;

    namespace lang {
        struct ModuleConfiguration {
            ScriptCache& scriptCache;
            TypeBuilder& typeBuilder;
            SymbolTableUpdater& SymbolTableUpdater;
            const ReservedKeywordsPool& reservedKeywords;
            StatementParser& parser;
            Interpreter& interpreter;
        };

        class Module {
        public:
            Module(ModuleConfiguration& config, std::string moduleName);    
            virtual ~Module() = default;
        protected:
            ScriptBridge m_bridge;
        };
    }
}

#pragma once

#include "Runtime/Service/ScriptBinding.h"
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

        template <class Interpreter>
        class Module {
        public:
            Module(ModuleConfiguration<Interpreter>& config, std::string moduleName):
                m_bridge{ config.scriptCache, config.scriptAstCache, std::move(moduleName), config.typeBuilder, config.symbolTableUpdater, config.reservedKeywords } {
            }

            virtual ~Module() = default;

        protected:
            using ScriptBridge = ScriptBinding<typename InterpreterTypes<Interpreter>::Script, typename InterpreterTypes<Interpreter>::ScriptCache>;
            ScriptBridge m_bridge;
        };
    }
}

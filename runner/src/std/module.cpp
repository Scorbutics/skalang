#include "Config/LoggerConfigLang.h"
#include "module.h"

ska::lang::Module::Module(ModuleConfiguration& config, std::string moduleName) 
    : m_bridge{ config.scriptCache, std::move(moduleName), config.typeBuilder, config.symbolTableTypeUpdater, config.reservedKeywords } {
}

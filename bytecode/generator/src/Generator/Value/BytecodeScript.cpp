#include <sstream>
#include "Config/LoggerConfigLang.h"
#include "BytecodeScript.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::bytecode::ScriptGenerationService);

ska::bytecode::ScriptGenerationService::ScriptGenerationService(ska::ScriptAST& script) :
	m_script(script.handle()) {
}

ska::bytecode::Register ska::bytecode::ScriptGenerationService::queryNextRegister() {
	return { VariableRef { m_register++ }, ValueType::REG };
}


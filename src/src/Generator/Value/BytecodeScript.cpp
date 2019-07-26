#include "Config/LoggerConfigLang.h"
#include "BytecodeScript.h"
#include "Interpreter/Value/Script.h"

ska::bytecode::Script::Script(ska::Script& script) :
	m_script(script.handle()) {
}

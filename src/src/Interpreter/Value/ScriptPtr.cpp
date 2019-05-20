#include "Config/LoggerConfigLang.h"
#include "ScriptPtr.h"
#include "Script.h"

ska::ScriptPtr::ScriptPtr(std::unique_ptr<Script> s) : std::unique_ptr<Script>{std::move(s)} { }

ska::ScriptPtr::~ScriptPtr() {}

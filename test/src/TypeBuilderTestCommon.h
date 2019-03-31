#pragma once
#include "NodeValue/AST.h"
#include "DataTestContainer.h"
#include "Interpreter/Value/Script.h"

ska::Script TypeBuilderTestCommonBuildAST(std::unordered_map<std::string, ska::ScriptHandlePtr>& scriptCache, const std::string& input, DataTestContainer& data, bool buildType = true);
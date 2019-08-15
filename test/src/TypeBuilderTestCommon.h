#pragma once
#include "NodeValue/AST.h"
#include "DataTestContainer.h"
#include "NodeValue/ScriptAST.h"

ska::ScriptAST TypeBuilderTestCommonBuildAST(std::unordered_map<std::string, ska::ScriptHandleASTPtr>& scriptCache, const std::string& input, DataTestContainer& data, bool buildType = true);
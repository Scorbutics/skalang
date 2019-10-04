#pragma once
#include "NodeValue/AST.h"
#include "DataTestContainer.h"
#include "NodeValue/ScriptAST.h"

ska::ScriptAST TypeBuilderTestCommonBuildAST(ska::ScriptCacheAST& scriptCache, const std::string& input, DataTestContainer& data, bool buildType = true);
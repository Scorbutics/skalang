#pragma once
#include "NodeValue/AST.h"
#include "DataTestContainer.h"

std::unique_ptr<ska::ASTNode> TypeBuilderTestCommonBuildAST(const std::string& input, DataTestContainer& data, bool buildType = true);
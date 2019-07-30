#pragma once
#include <cassert>
#include "Generator/Value/BytecodeInstruction.h"

namespace ska {
  class ASTNode;
	namespace bytecode {
    struct TypeConversionData {
      const ASTNode* node = nullptr;
      Type type;
      Command command;
    };
    TypeConversionData TypeConversionBinary(const ASTNode& node1, const ASTNode& node2, const Type& destinationType);
	}
}

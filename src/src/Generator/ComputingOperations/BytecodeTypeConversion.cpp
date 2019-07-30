#include "NodeValue/AST.h"
#include "BytecodeTypeConversion.h"

namespace ska {
  namespace bytecode {
    static const ASTNode* ConvertWhich(const Type& destinationType, const ASTNode& node1, const ASTNode& node2) {
      if(node1.type() != destinationType && node2.type() != destinationType) {
        throw std::runtime_error("unable to perform node conversion");
      }

      if(node1.type() == node2.type()) {
        return nullptr;
      }
      return node1.type() == destinationType ? &node2 : &node1;
    }

    Command PerformConversion(const ASTNode& node, const Type& destinationType) {
      return Command::NOP;
    }
  }
}

ska::bytecode::TypeConversionData ska::bytecode::TypeConversionBinary(const ASTNode& node1, const ASTNode& node2, const Type& destinationType) {
  const auto* node = ConvertWhich(destinationType, node1, node2);
  return node == nullptr ? TypeConversionData {} : PerformConversion(*node, destinationType);
}

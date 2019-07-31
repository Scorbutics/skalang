#include "NodeValue/AST.h"
#include "BytecodeTypeConversion.h"
#include "NodeValue/Type.h"

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

    template <class Value>
    static void EnqueueConverterItem(TypeConvertToInstructionContainer& container, LogicalOperator op, ExpressionType t1, ExpressionType t2, Value&& value) {
      container[static_cast<std::size_t>(op)][static_cast<std::size_t>(t1)][static_cast<std::size_t>(t2)] = std::forward<Value>(value);
    }

    static TypeConvertToInstructionContainer BuildTypeConverter() {
      auto result = TypeConvertToInstructionContainer{};

      EnqueueConverterItem(result, LogicalOperator::ADDITION, ExpressionType::INT, ExpressionType::STRING, Command::CONV_I_STR);
      EnqueueConverterItem(result, LogicalOperator::ADDITION, ExpressionType::FLOAT, ExpressionType::STRING, Command::CONV_D_STR);
      EnqueueConverterItem(result, LogicalOperator::ADDITION, ExpressionType::INT, ExpressionType::FLOAT, Command::CONV_I_D);
      EnqueueConverterItem(result, LogicalOperator::ADDITION, ExpressionType::INT, ExpressionType::ARRAY, [](const Type& t, const Type& dest) { if(dest.compound()[0] != t) {throw std::runtime_error("bad + conversion"); } return Command::PUSH_B_ARR; });

      return result;
    }

    static TypeConvertToInstructionContainer typeConverter = BuildTypeConverter();
  }
}

ska::bytecode::TypeConversionData ska::bytecode::TypeConversionBinary(const ASTNode& node1, const ASTNode& node2, const Type& destinationType) {
  const auto* node = ConvertWhich(destinationType, node1, node2);
  //return node == nullptr ? TypeConversionData {} : PerformConversion(*node, destinationType);
  return {};
}

std::optional<ska::bytecode::Instruction> ska::bytecode::TypeConversion(LogicalOperator logicalOperator, const Value& value, const Type& valueType, const Type& destinationType) {
  if(valueType == destinationType) {
    return {};
  }

  const auto operatorIndex = static_cast<std::size_t>(logicalOperator);
  const auto nodeExpressionTypeIndex = static_cast<std::size_t>(valueType.type());
  const auto resultExpressionTypeIndex = static_cast<std::size_t>(destinationType.type());

  auto& converter = typeConverter[operatorIndex][nodeExpressionTypeIndex][resultExpressionTypeIndex];
  if(std::holds_alternative<Command>(converter)) {
    auto& command = std::get<Command>(converter);
    return command == Command::NOP ? Instruction{} : Instruction { command, value };
  }
  auto& converterCallback = std::get<TypeToInstructionConverterCallback>(converter);

  return converterCallback ? Instruction{ converterCallback(valueType, destinationType), value} : std::optional<Instruction>{};
}

#pragma once
#include <optional>
#include <cassert>
#include <functional>
#include <array>
#include <variant>
#include "Generator/Value/BytecodeInstruction.h"
#include "NodeValue/LogicalOperator.h"
#include "NodeValue/ExpressionType.h"

namespace ska {
  class ASTNode;
	namespace bytecode {
    struct TypeConversionData {
      const ASTNode* node = nullptr;
      Type type;
      Command command;
    };
    using TypeToInstructionConverterCallback = std::function<Command(const Type&, const Type&)>;
    using TypeToInstructionConverter = std::variant<Command, TypeToInstructionConverterCallback>;
    using TypeToInstructionConverterContainer = std::array<TypeToInstructionConverter, static_cast<std::size_t>(ExpressionType::UNUSED_Last_Length)>;
    using TwoTypesConverterContainer = std::array<TypeToInstructionConverterContainer, static_cast<std::size_t>(ExpressionType::UNUSED_Last_Length)>;
    using TypeConvertToInstructionContainer = std::array<TwoTypesConverterContainer, static_cast<std::size_t>(LogicalOperator::UNUSED_Last_Length)>;

    TypeConversionData TypeConversionBinary(const ASTNode& node1, const ASTNode& node2, const Type& destinationType);

    std::optional<Instruction> TypeConversion(LogicalOperator logicalOperator, const Value& value, const Type& valueType, const Type& destinationType);
	}
}

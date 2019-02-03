#include "TypeBuilderBridge.h"
#include "NodeValue/AST.h"

ska::Type ska::TypeBuilderOperator<ska::Operator::BRIDGE>::build(const SymbolTable& symbols, OperateOn node) {
    return node.GetValue().type().value();
}

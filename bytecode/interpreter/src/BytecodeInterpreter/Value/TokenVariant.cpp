#include "TokenVariant.h"

ska::bytecode::TokenVariant ska::bytecode::TokenVariantFromValue(const Value& value) {
  auto output = TokenVariant {};
  std::visit([&output](const auto& content){
    using TypeT = std::decay_t<decltype(content)>;
    if constexpr(std::is_same_v<StringShared, TypeT>) {
      output = content;
    } else if constexpr(std::is_same_v<long, TypeT>) {
      output = content;
    } else if constexpr(std::is_same_v<double, TypeT>) {
      output = content;
    } else if constexpr(std::is_same_v<std::size_t, TypeT>) {
      output = content;
    } else if constexpr(std::is_same_v<bool, TypeT>) {
      output = content;
    } else {
      TypeT::lol;
      throw std::runtime_error("error variant type");
    }
  }, value.content);
  return output;
};

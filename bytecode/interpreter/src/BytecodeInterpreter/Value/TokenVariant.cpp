#include "TokenVariant.h"

ska::bytecode::TokenVariant ska::bytecode::TokenVariantFromValue(const Value& value) {
  auto& content = value.content;
  using TypeT = std::decay_t<decltype(content)>;
  if constexpr(std::is_same_v<StringShared, TypeT>) {
    return TokenVariant {std::get<StringShared>(content)};
  } else if constexpr(std::is_same_v<long, TypeT>) {
    return TokenVariant {std::get<long>(content)};
  } else if constexpr(std::is_same_v<double, TypeT>) {
    return TokenVariant {std::get<double>(content)};
  } else if constexpr(std::is_same_v<std::size_t, TypeT>) {
    return TokenVariant {std::get<std::size_t>(content)};
  } else if constexpr(std::is_same_v<bool, TypeT>) {
    return TokenVariant {std::get<bool>(content)};
  }
  throw std::runtime_error("error variant");
};

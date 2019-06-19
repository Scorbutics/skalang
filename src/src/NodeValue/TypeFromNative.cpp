#include "TypeFromNative.h"

const ska::Type ska::TypeFromNative<int>::value = ska::Type::MakeBuiltIn<ska::ExpressionType::INT>();
const ska::Type ska::TypeFromNative<float>::value = ska::Type::MakeBuiltIn<ska::ExpressionType::FLOAT>();
const ska::Type ska::TypeFromNative<double>::value = ska::Type::MakeBuiltIn<ska::ExpressionType::FLOAT>();
const ska::Type ska::TypeFromNative<bool>::value = ska::Type::MakeBuiltIn<ska::ExpressionType::BOOLEAN>();
const ska::Type ska::TypeFromNative<ska::StringShared>::value = ska::Type::MakeBuiltIn<ska::ExpressionType::STRING>();
const ska::Type ska::TypeFromNative<ska::NodeValueArray>::value = ska::Type::MakeBuiltIn<ExpressionType::ARRAY>();
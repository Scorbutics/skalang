#pragma once
#include <unordered_map>

namespace ska {
    enum class LogicalOperator {
        ADDITION,
        MULTIPLY,
        DIVIDE,
        SUBSTRACT,
        EQUAL,
        CONDITION,
        NOT_CONDITION,
        NOT,
        ADDITION_EQUAL,
        SUBSTRACT_EQUAL,
        MULTIPLY_EQUAL,
        DIVIDE_EQUAL
    };

    static const auto LogicalOperatorMap = std::unordered_map<std::string, LogicalOperator>{
        {"+",  LogicalOperator::ADDITION},
        {"*",  LogicalOperator::MULTIPLY},
        {"/",  LogicalOperator::DIVIDE},
        {"-",  LogicalOperator::SUBSTRACT},
        {"=",  LogicalOperator::EQUAL},
        {"==", LogicalOperator::CONDITION},
        {"!=", LogicalOperator::NOT_CONDITION},
        {"!",  LogicalOperator::NOT},
        {"+=", LogicalOperator::ADDITION_EQUAL},
        {"-=", LogicalOperator::SUBSTRACT_EQUAL},
        {"*=", LogicalOperator::MULTIPLY_EQUAL},
        {"/=", LogicalOperator::DIVIDE_EQUAL}
    };
}

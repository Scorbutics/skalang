#include "InterpreterCommandSubArr.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(SUB_ARR)(ExecutionContext& context, const Value& left, const Value& right) {
  auto& leftSide = *context.get<NodeValueArray>(left);
	auto& rightSide = *context.get<NodeValueArray>(right);

  if(rightSide.empty()) {
    return context.get<NodeValueArray>(left);
  }

  if(rightSide.size() > 2) {
    throw std::runtime_error("invalid array range deletion with an array size of " + std::to_string(rightSide.size()));
  }

  const auto firstVal = rightSide[0].nodeval<long>();
  if(firstVal < 0) {
    throw std::runtime_error("invalid array range deletion : the first value is negative");
  }

  long lastVal;
  if(rightSide.size() == 2) {
    lastVal = rightSide[1].nodeval<long>();
    if(lastVal < 0) {
      throw std::runtime_error("invalid array range deletion : the last value is negative");
    } else if(firstVal > lastVal) {
      throw std::runtime_error("invalid array range deletion : the first array value is greater than the last.");
    }
  } else {
    lastVal = firstVal;
  }

  auto container = context.get<NodeValueArray>(left);
  for(std::size_t i = 0; i < leftSide.size(); i = (i >= firstVal && i <= lastVal) ? lastVal + 1 : i + 1 ) {
    container->push_back(leftSide[i]);
  }
	return container;

}

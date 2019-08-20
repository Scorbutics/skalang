#include "InterpreterCommandPopFArr.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(POP_F_ARR)(ExecutionContext& context, Value& left, Value& right) {
  auto numberToPop = context.get<long>(left);
	auto container = context.get<NodeValueArray>(right);

  if(numberToPop >= container->size()) {
    container->clear();
  } else {
    container->erase(container->begin(), container->begin() + numberToPop);
  }

	return container;

}

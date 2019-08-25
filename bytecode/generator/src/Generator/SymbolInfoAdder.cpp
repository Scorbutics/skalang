#include "SymbolInfoAdder.h"
#include "Value/BytecodeGenerationOutput.h"
#include "NodeValue/AST.h"
#include "NodeValue/OperatorTraits.h"

/*
void ska::bytecode::SymbolInfoAdder::process(GenerationOutput& generated) {
	for (const auto& node : root) {
		processUnit(generated, root);
		process(generated, *node);
	}
}
*/

void ska::bytecode::SymbolInfoAdder::processUnit(GenerationOutput& generated, const ASTNode& root) {
	if (OperatorTraits::isNamed(root.op())) {

	}
}

#include "BytecodeClosure.h"
#include "NodeValue/AST.h"
#include "Service/ScopedSymbolTable.h"
#include "../BytecodeGenerationContext.h"

ska::bytecode::Closure::Closure(const ASTNode& scopeNode):
	m_scopeNode(&scopeNode) {
	assert(m_scopeNode->symbol() != nullptr);
}

ska::bytecode::InstructionOutput ska::bytecode::Closure::checkAndCapture(GenerationContext& context, const ScopedSymbolTable* variable, Operand variableOperand) {
	if (variable == nullptr) {
		return {};
	}

	const auto& scope = *m_scopeNode;
	const auto closureVariableMatch = &variable->parent() != scope.symbolTable() && variable != scope.symbolTable();
	if (closureVariableMatch && m_environment.find(variable) == m_environment.end()) {
		// TODO we might concat every USE_ENV bytecode command contents at the beginning of the function execution (after JUMP_REL)
		// instead of having several USE_ENV dispatched in the whole function body
		auto result = Instruction { Command::USE_ENV, variableOperand };
		m_environment.emplace(variable, (const ScopedSymbolTable*) variable);
		return result;
	}
	return {};
}

ska::bytecode::InstructionOutput ska::bytecode::Closure::generate(const GenerationContext& context) const {
	if (m_environment.empty()) {
		return {};
	}

	auto output = InstructionOutput {};

	for (const auto& node: m_environment) {
		// Ex: declare "count" to be in closure V2 env as env variable index "0"
		// => [ADD_ENV|V2:4|V1:4]
		auto operand = context.getSymbol(*node->symbol());
		output.push(Instruction { Command::ADD_ENV, operand.value() });
	}
	return output;
}

#include "BytecodeClosure.h"
#include "NodeValue/AST.h"
#include "Service/ScopedSymbolTable.h"
#include "../BytecodeGenerationContext.h"

ska::bytecode::Closure::Closure(const ASTNode& scopeNode, Closure* parent):
	m_scopeNode(&scopeNode),
	m_parent(parent) {
	assert(m_scopeNode->symbol() != nullptr);
}

ska::bytecode::InstructionOutput ska::bytecode::Closure::checkAndCapture(GenerationContext& context, const ScopedSymbolTable* variable, Operand variableOperand) {
	if (variable == nullptr || m_parent == nullptr) {
		return {};
	}

	const auto& scope = *m_scopeNode;
	const auto closureVariableMatch = &variable->parent() != scope.symbolTable() && variable != scope.symbolTable();
	if (closureVariableMatch && m_parent->m_environment.find(variable) == m_environment.end()) {
		// TODO find the real closure place.
		// We need to create a closure at the targetted symbol table, not the current one!!
		// That's why it's currently not working...
		auto result = Instruction { Command::USE_ENV, variableOperand };
		m_parent->m_environment.emplace(variable, (const ScopedSymbolTable*) variable);
		return result;
	}
	return {};
}

ska::bytecode::InstructionOutput ska::bytecode::Closure::generate(const GenerationContext& context) const {
	if (m_environment.empty()) {
		return {};
	}

	auto output = InstructionOutput {};

	// TODO find the real closure place.
	// We need to create a closure at the targetted symbol table, not the current one!!
	// That's why it's currently not working...
	// Using m_environment we could retrieve the real closure place.
	for (const auto& node: m_environment) {
	//if (m_environment.find(context.pointer().symbolTable()) != m_environment.end()) {
		auto operand = context.getSymbol(*node->symbol());
		output.push(Instruction { Command::ADD_ENV, operand.value() });
	}
	return output;
}

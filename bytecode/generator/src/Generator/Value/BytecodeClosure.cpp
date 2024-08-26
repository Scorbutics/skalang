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

	const auto closureVariableMatch = &variable->parent() != m_scopeNode->symbolTable() && variable != m_scopeNode->symbolTable();
	if (closureVariableMatch && m_environment.find(variable) == m_environment.end()) {
		auto closureOperand = context.querySymbolOrOperand(*m_scopeNode).operand();
		auto result = Instruction { Command::USE_ENV, variableOperand, closureOperand, Operand { static_cast<long>(m_environment.size()), OperandType::PURE } };
		m_environment.emplace(variable);
		return result;
	}
	return {};
}

ska::bytecode::InstructionOutput ska::bytecode::Closure::generate(const GenerationContext& context) const {
	if (m_environment.empty()) {
		return {};
	}

	auto output = InstructionOutput {};

	const auto closureOperand = context.getSymbol(*m_scopeNode->symbol());
	if (!closureOperand.has_value()) {
		throw std::runtime_error("unable to retrieve operand for closure symbol \"" + m_scopeNode->symbol()->name() + "\"");
	}

	for (const auto& node: m_environment) {
		// Declare "count" to be in closure V2 env as env variable index "0"
		// [ADD_ENV|V2:4|V1:4]
		auto operand = context.getSymbol(*node->symbol());
		output.push(Instruction { Command::ADD_ENV, closureOperand.value(), operand.value() });
	}
	//m_environment.clear();
	return output;
}
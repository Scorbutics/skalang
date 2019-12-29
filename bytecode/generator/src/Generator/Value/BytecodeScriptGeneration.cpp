#include "Config/LoggerConfigLang.h"
#include "BytecodeScriptGeneration.h"
#include "Generator/BytecodeGenerator.h"

static ska::bytecode::ScriptGenerationHelper AddScript(ska::bytecode::ScriptCache& cache, std::vector<ska::Token> tokens, const std::string& name) {
	auto ast = ska::ScriptAST{ cache.astCache, name, std::move(tokens) };
	return { cache, ast };
}

ska::bytecode::ScriptGeneration::ScriptGeneration(ScriptCache& cache, std::vector<ska::Token> tokens, const std::string& name) :
	ScriptGeneration(AddScript(cache, std::move(tokens), name)) {
}

ska::bytecode::ScriptGeneration::ScriptGeneration(ScriptGenerationHelper origin, InstructionOutput instruction) :
	m_origin(std::move(origin)),
	m_generated(std::move(instruction)) {
}

const ska::ASTNode& ska::bytecode::ScriptGeneration::rootASTNode() const {
	return m_origin.program().rootNode();
}

std::ostream& ska::bytecode::operator<<(std::ostream& stream, const ScriptGeneration& output) {
	stream << output.m_generated;
	return stream;
}

void ska::bytecode::ScriptGeneration::generate(ScriptCache& cache, Generator& generator) {
	auto context = GenerationContext{ cache, m_origin.program() };
	m_generated = generator.generatePart(context);
}

std::optional<ska::bytecode::Operand> ska::bytecode::ScriptGeneration::getSymbol(const Symbol& symbol) const {
	return m_origin.getSymbol(symbol);
}


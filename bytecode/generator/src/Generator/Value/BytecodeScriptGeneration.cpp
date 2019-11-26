#include "Config/LoggerConfigLang.h"
#include "BytecodeScriptGeneration.h"
#include "Generator/BytecodeGenerator.h"

const ska::ASTNode& ska::bytecode::ScriptGeneration::rootASTNode() const {
	return m_origin.program().rootNode();
}

std::ostream& ska::bytecode::operator<<(std::ostream& stream, const ScriptGeneration& output) {
	stream << output.m_generated;
	return stream;
}

void ska::bytecode::ScriptGeneration::generate(ScriptCache& cache, Generator& generator) {
	auto context = GenerationContext{ cache, m_origin.id() };
	m_generated = generator.generatePart(context);
}


ska::ScriptASTPtr ska::bytecode::ScriptGeneration::useImport(const std::string& scriptImported) {
	return m_origin.program().useImport(scriptImported);
}

#include "BytecodeScript.h"

#include "BytecodeInterpreter/BytecodeInterpreter.h"
#include "Generator/BytecodeGenerator.h"

void ska::bytecode::Script::generate(Generator& generator) {
  if (m_generated) {
    return;
  }

  //TODO c'est lourd...

  m_id = m_generation.size();
  m_generation.push(std::move(m_serviceGen.service));

  auto genContext = GenerationContext { m_generation };
  auto scriptOutput = generator.generatePart(genContext);
  //TODO impossible de faire cette étape automatiquement ? c'est lourd...
  m_generation.setOut(m_id, std::move(scriptOutput));

  m_generated = true;
}

std::unique_ptr<ska::bytecode::Executor> ska::bytecode::Script::execute(Interpreter& interpreter) {
  return interpreter.interpret(m_id, m_generation);
}

#include "Config/LoggerConfigLang.h"
#include "ObjectFieldAccessReplacer.h"
#include "Value/BytecodeGenerationOutput.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::ObjectFieldAccessReplacer);

#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::ObjectFieldAccessReplacer)

void ska::bytecode::ObjectFieldAccessReplacer::process(GenerationOutput& generated) {
	std::unordered_set<std::size_t> valuesToWatch;

	const auto& symbols = generated.symbols();

	for (auto instruction = generated.begin(); instruction != generated.end(); instruction++) {
		auto container = std::unordered_map<std::size_t, std::size_t>{};

		switch (instruction->command()) {
			case Command::POP_IN_VAR: {
				const auto instructionIndex = std::distance(generated.begin(), instruction);
				const auto& fieldsInObject = symbols[instructionIndex].references;
				markFields(valuesToWatch, instruction->dest(), fieldsInObject);
				LOG_INFO << "Pop in var at instruction " << instructionIndex << " with fields object " << fieldsInObject;
			}	break;
		}
	}
}

void ska::bytecode::ObjectFieldAccessReplacer::markFields(std::unordered_set<std::size_t>& valuesToWatch, const Value& valueToWatch, FieldsReferences fields) {


}

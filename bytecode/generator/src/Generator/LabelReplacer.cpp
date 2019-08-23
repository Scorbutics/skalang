#include "Config/LoggerConfigLang.h"
#include "LabelReplacer.h"
#include "Value/BytecodeGenerationOutput.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::bytecode::LabelReplacer);

#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::LabelReplacer)

void ska::bytecode::LabelReplacer::process(GenerationOutput& generated) {
	auto container = LabelContainer{};

	for(auto instruction = generated.begin(); instruction != generated.end(); ) {
		if(instruction->command() == Command::LABEL) {
			const auto labelRefIndex = std::get<std::size_t>(instruction->dest().as<VariableRef>());
			const auto instructionIndex = std::distance(generated.begin(), instruction);
			insertLabel(container, labelRefIndex, instructionIndex);
			LOG_INFO << "Label " << labelRefIndex << " found at instruction " << container[labelRefIndex];
			instruction = generated.erase(instruction);
		} else {
			if(instruction->command() == Command::END) {
				const auto labelRefIndex = std::get<std::size_t>(instruction->left().as<VariableRef>());
				LOG_INFO << "Label " << labelRefIndex << " found as end function at instruction " << std::distance(generated.begin(), instruction);
				assert(labelRefIndex < container.size());
				(*instruction) = Instruction{ Command::END, instruction->dest(), Value{static_cast<long>(container[labelRefIndex])}, instruction->right() };
			}
			instruction++;
		}
	}
}

void ska::bytecode::LabelReplacer::insertLabel(LabelContainer& container, std::size_t labelRef, std::size_t generationIndex) {
	if(labelRef == container.size()) {
		container.push_back(generationIndex);
		return;
	}

	if(labelRef > container.size()) {
		container.resize(labelRef + 1);
	}
	container[labelRef] = generationIndex;
}

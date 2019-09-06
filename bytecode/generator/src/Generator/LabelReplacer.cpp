#include "Config/LoggerConfigLang.h"
#include "LabelReplacer.h"
#include "Value/BytecodeGenerationOutput.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::LabelReplacer);

#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::LabelReplacer)

void ska::bytecode::LabelReplacer::process(GenerationOutput& generated) {
	auto container = LabelContainer{};

	for (auto instruction = generated.begin(); instruction != generated.end(); instruction++) {
		switch (instruction->command()) {
		case Command::END: {
			const auto functionStartRelativeInstruction = instruction->left().as<long>() + 1;
			const auto variableRefIndex = std::get<std::size_t>(instruction->dest().as<VariableRef>());
			const auto instructionIndex = std::distance(generated.begin(), instruction);
			insertLabel(container, variableRefIndex, functionStartRelativeInstruction + instructionIndex);
			LOG_INFO << "End " << variableRefIndex << " found at instruction " << container[variableRefIndex];
		}	break;

		case Command::JUMP_ABS: {
			const auto variableRefIndex = std::get<std::size_t>(instruction->dest().as<VariableRef>());
			LOG_INFO << "Jump call " << variableRefIndex << " found as end function at instruction " << std::distance(generated.begin(), instruction);
			assert(variableRefIndex < container.size());
			(*instruction) = Instruction{ Command::JUMP_ABS, Value{static_cast<long>(container[variableRefIndex])} };
		}	break;

		default:
			break;
		}
	}
	
}

void ska::bytecode::LabelReplacer::insertLabel(LabelContainer& container, std::size_t variableRef, std::size_t generationIndex) {
	if(variableRef == container.size()) {
		container.push_back(generationIndex);
		return;
	}

	if(variableRef > container.size()) {
		container.resize(variableRef + 1);
	}
	container[variableRef] = generationIndex;
}

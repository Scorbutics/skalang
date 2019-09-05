#include <sstream>
#include <deque>
#include "Config/LoggerConfigLang.h"
#include "ObjectFieldAccessReplacer.h"
#include "Value/BytecodeGenerationOutput.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::bytecode::ObjectFieldAccessReplacer);

#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::ObjectFieldAccessReplacer)
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::ObjectFieldAccessReplacer)

namespace ska {
	namespace bytecode {
		template <class Iterator>
		static bool ExpectNextInstructionCommand(Iterator&& instruction, std::deque<Command>& expectedNextCommands) {
			if(!expectedNextCommands.empty()) {
				if(expectedNextCommands.front() != instruction->command()) {
					auto ss = std::stringstream {};
					ss << "invalid bytecode command : the current instruction was expected to have a " << expectedNextCommands.front() << " command";
					throw std::runtime_error(ss.str());
				}
				expectedNextCommands.pop_front();
				return true;
			}
			return false;
		}
	}
}

/*
Example :

TEST_CASE("[BytecodeGenerator] Custom object creation") {
	constexpr auto progStr =
		"var toto = function() : var {"
			"var priv_test = 1;"
			"return {"
				"test : priv_test,"
				"say : function(more : string) : string {"
					"var s = \"lol\" + priv_test + more;"
					"return s;"
				"}"
			"};"
		"};"
		"var test = toto();"
		"test.test;";

	auto [astPtr, data] = ASTFromInputBytecodeGenerator(progStr);
	auto res = data.generator->generate(astPtr);

	BytecodeCompare(res, {
		{ska::bytecode::Command::JUMP_REL, "14"},
		{ska::bytecode::Command::MOV, "V0", "1"},
		{ska::bytecode::Command::MOV, "V1", "V0"},
		{ska::bytecode::Command::PUSH, "V1"},
		{ska::bytecode::Command::JUMP_REL, "6"},
		{ska::bytecode::Command::POP, "V2"},
		{ska::bytecode::Command::CONV_I_STR, "R0", "V0"},
		{ska::bytecode::Command::ADD_STR, "R0", "R0", "V2"},
		{ska::bytecode::Command::ADD_STR, "R1", "lol", "R0"},
		{ska::bytecode::Command::MOV, "V3", "R1"},
		{ska::bytecode::Command::RET, "V3"},
		{ska::bytecode::Command::END, "V4", "-7"},
		{ska::bytecode::Command::PUSH, "V4"},
		{ska::bytecode::Command::POP_IN_VAR, "R2", "2"}, // store as "symbol info" = relative references map : V1 : 0, V4 : 1. Remark : POP_IN_VAR is always followed by RET and END => references are in V5
		{ska::bytecode::Command::RET, "R2"},
		{ska::bytecode::Command::END, "V5", "-15"},
		{ska::bytecode::Command::JUMP_ABS, "V5"}, // JUMP_ABS is followed by a single POP in case of object creation. so the resulting references will be stored in R3
		{ska::bytecode::Command::POP, "R3"},
		{ska::bytecode::Command::MOV, "V6", "R3"}, // MOV an object-tagged register : R3 in V6
		{ska::bytecode::Command::ARR_ACCESS, "R4", "V6", "0"}, // "test" is V1, V1 is 0th index
	});
}
*/

void ska::bytecode::ObjectFieldAccessReplacer::process(GenerationOutput& generated) {
	auto valuesToWatch = ValueWatcher{};

	//const auto& symbols = generated.symbols();

	auto expectedNextCommands = std::deque<Command>{};

	auto objectFieldsMap = std::make_shared<FieldsReferencesRaw>();

	for (auto instruction = generated.begin(); instruction != generated.end(); instruction++) {

		if(!expectedNextCommands.empty()) {
			LOG_DEBUG << "Reading instruction with command : " << instruction->command() << " and expecting " << expectedNextCommands.front();
		}

		const auto wasExpected = ExpectNextInstructionCommand(instruction, expectedNextCommands);

		switch (instruction->command()) {
			case Command::POP_IN_VAR: {
				expectedNextCommands.push_back(Command::RET);
				expectedNextCommands.push_back(Command::END);
			}	break;

			case Command::MOV:
				// Handles MOV : Tracks object reference duplications
				if(valuesToWatch.find(instruction->left()) != valuesToWatch.end()) {
					valuesToWatch.emplace(instruction->dest());
				} else if(valuesToWatch.find(instruction->dest()) != valuesToWatch.end()) {
					// When reassign a preexisting variable reference (it's not supposed to be allowed because OBJECT type is not meant to be reassignable, but in case one day it would be...)
					valuesToWatch.erase(instruction->dest());
				}
			break;

			case Command::END: {
				// Handles END : Tracks object reference creation (class-like factory method)
				const auto instructionIndex = std::distance(generated.begin(), instruction);
				/*
				const auto& fieldsInObject = symbols[instructionIndex].references;
				LOG_INFO << "END at instruction " << instructionIndex << " with fields object : ";
				if(fieldsInObject == nullptr) {
					throw std::runtime_error("invalid object creation : no field are referenced in this return value (instruction " + std::to_string(instructionIndex) + ")");
				}
				for(const auto& [key, value] : *fieldsInObject) {
					LOG_INFO << key << " : " << value;
				}

				objectFieldsMap = fieldsInObject;
				valuesToWatch.emplace(instruction->dest());
				*/
			} break;

			case Command::POP:
				// When expected, those must be tracked : they are part of return value when the factory function is called
				if(wasExpected) {
					valuesToWatch.emplace(instruction->dest());
				}
			break;

			case Command::JUMP_ABS: {
				expectedNextCommands.push_back(Command::POP);
			} break;

			case Command::ARR_ACCESS: {
				// Finally, that command is the goal of this class. When encountered, replace the right operand variable reference
				// (which contains the field) of the left one (which contains the object) by its field index in object.
				auto fieldRef = instruction->right();
				auto objectRef = instruction->left();
				auto destinationRef = instruction->dest();

				// This case is to avoid (classic array access by index)
				if(fieldRef.type() == ValueType::PURE) {
					break;
				}

				const auto fieldRefIndex = std::get<std::size_t>(fieldRef.as<VariableRef>());

				if(valuesToWatch.find(objectRef) == valuesToWatch.end()) {
					throw std::runtime_error("unable to replace the field of this object access (unknown object \"" + objectRef.toString() + "\" )");
				}

				if(objectFieldsMap == nullptr || objectFieldsMap->find(fieldRefIndex) == objectFieldsMap->end()) {
					throw std::runtime_error("unable to replace the field of this object access (unknown field \"" + fieldRef.toString() + "\" of object \"" + objectRef.toString() + "\" )");
				}
				(*instruction) = Instruction { Command::ARR_ACCESS, destinationRef, objectRef, Value { VariableRef { objectFieldsMap->at(fieldRefIndex) } } };

				const auto instructionIndex = std::distance(generated.begin(), instruction);
				LOG_INFO << "Replacing array access at instruction " << instructionIndex << " with instruction " << (*instruction);
			} break;
		}
	}
}

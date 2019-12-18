#include <string>
#include "NodeValue/AST.h"
#include "GeneratorOperatorFieldAccess.h"
#include "NodeValue/ScriptAST.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScriptGenerationHelper.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::GeneratorOperator<ska::Operator::FIELD_ACCESS>);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::FIELD_ACCESS>)

ska::bytecode::InstructionOutput ska::bytecode::GeneratorOperator<ska::Operator::FIELD_ACCESS>::generate(OperateOn node, GenerationContext& context) {
	const auto typeObject = node.GetObjectType();
	const auto& fieldName = node.GetFieldNameNode().name();
	const auto* symbolField = typeObject[fieldName];
	if (symbolField == nullptr || !typeObject.hasSymbol()) {
		auto ss = std::stringstream{};
		ss << "trying to access to an undeclared field : \"" << fieldName << "\" of \"" << node.GetObjectNameNode().name() << "\"";
        throw std::runtime_error(ss.str());
	}

	auto optFieldValue = context.getSymbol(*symbolField);
	if (optFieldValue.has_value()) {
		LOG_DEBUG << "Accessing field " << optFieldValue.value() << " of object " << node.GetObjectNameNode();
	} else {
		auto ss = std::stringstream{ };
		ss << "invalid bytecode : cannot access field \"" << symbolField->getName() << " of object " << node.GetObjectNameNode() << " in script \"" << context.scriptName() << "\"";
		throw std::runtime_error(ss.str());		
	}

	auto objectValue = generateNext({ context, node.GetObjectNameNode()});
	const auto* objectSymbolInfo = context.getSymbolInfo(*symbolField);
	if(objectSymbolInfo == nullptr) {
		auto ss = std::stringstream { };
		ss << "invalid bytecode : the dereferenced object \"" << node.GetObjectNameNode() << "\" is not registered";
		throw std::runtime_error(ss.str());
	}
	const auto objectFieldReferences = objectSymbolInfo->references;
	if(objectFieldReferences == nullptr || objectFieldReferences->empty()) {
		throw std::runtime_error("invalid bytecode : the dereferenced object has no fields references");
	}

	auto fieldValue = optFieldValue.value();
	const auto fieldVarReference = fieldValue.as<ScriptVariableRef>();
	LOG_DEBUG << "This field is " << fieldVarReference.variable << " in script index " << fieldVarReference.script;

	const auto fieldRefIndex = objectFieldReferences->find(fieldVarReference);
	if(fieldRefIndex == objectFieldReferences->end()) {
		throw std::runtime_error("invalid bytecode : the field \"" + fieldValue.toString()
			+ "\" does not exist in object \"" + node.GetObjectNameNode().name() + "\" (" + objectValue.operand().toString() + ")");
	}

	LOG_DEBUG << "This field has index " << fieldRefIndex->second << " in the object";

	objectValue.push({ Instruction { Command::ARR_ACCESS, context.queryNextRegister(), objectValue.operand(), Operand {static_cast<long>(fieldRefIndex->second)} }});
	return objectValue;
}

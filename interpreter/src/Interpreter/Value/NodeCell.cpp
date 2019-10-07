#include "Config/LoggerConfigLang.h"
#include "NodeCell.h"
#include "Runtime/Value/ObjectMemory.h"
#include "Interpreter/Value/ScriptHandle.h"
#include "Interpreter/Value/Script.h"
#include "Interpreter/MemoryTable.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::NodeCell)

ska::NodeCell ska::NodeCell::build(MemoryLValue& memoryField) {
	if (isLvalue()) {
		return NodeLValue{ memoryField.first, memoryField.second };
	}
	return NodeRValue{ std::move(*memoryField.first), std::move(memoryField.second) };
}

ska::NodeCell ska::NodeCell::operator()(Script& parent, const std::string& key) {
	auto& val = isLvalue() ? *std::get<NodeValue*>(m_variant) : std::get<NodeValue>(m_variant);
	auto& variant = val.as<TokenVariant>();
	const auto isObject = std::holds_alternative<ObjectMemory>(variant);
	if (isObject) {
		auto memoryObject = std::get<ObjectMemory>(variant);
		assert(memoryObject != nullptr && "disallowed null memory object");
		auto memoryField = (*memoryObject)(key);
		return build(memoryField);
	}
	
	auto& scriptZoneId = std::get<ScriptVariableRef>(variant);
	SLOG(LogLevel::Debug) << "accessing script " << scriptZoneId.script << " for retrieving object named " << key;
	auto scriptZone = parent.useImport(scriptZoneId.script);
	auto& memoryScript = scriptZone->downMemory();
	auto memoryField = (memoryScript)(key);
	return build(memoryField);
}

ska::NodeCell::NodeCell() : 
	NodeCell(NodeRValue{ "", nullptr }) {
}

ska::NodeCell::NodeCell(NodeRValue rvalue) :
	m_memory(std::move(rvalue.memory)),
	m_variant(std::move(rvalue.object)) {
}

ska::NodeCell::NodeCell(NodeLValue lvalue) :
	m_memory(std::move(lvalue.memory)),
	m_variant(std::move(lvalue.object)) {
	assert(m_memory != nullptr && "lvalue not provided (null)");
}

ska::NodeLValue ska::NodeCell::asLvalue() {
	assert(isLvalue() && "Must be an lvalue");
	return { std::get<NodeValue*>(m_variant), m_memory };
}

ska::NodeRValue ska::NodeCell::asRvalue() {
	if (isLvalue()) {
		//Keeps a copy of the value stored in the nodecell when it's an lvalue to allow later reuse
		auto newValue = std::get<NodeValue*>(m_variant);
		return NodeRValue{
			std::move(newValue),
			m_memory
		};
	}
	return NodeRValue {
		std::move(std::get<NodeValue>(m_variant)),
		std::move(m_memory)
	};
	
}

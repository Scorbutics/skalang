#include "NodeCell.h"
#include "NodeValue/ObjectMemory.h"
#include "Service/ScriptHandle.h"
#include "MemoryTable.h"

ska::NodeCell ska::NodeCell::operator()(const std::string& key) {
	auto& val = isLvalue() ? *std::get<NodeValue*>(m_variant) : std::get<NodeValue>(m_variant);
	auto& variant = val.as<TokenVariant>();
	const auto isObject = std::holds_alternative<ObjectMemory>(variant);
	if (isObject) {
		auto memoryObject = std::get<ObjectMemory>(variant);
		assert(memoryObject != nullptr && "disallowed null memory object");
		auto memoryField = (*memoryObject)(key);
		return isLvalue() ? NodeCell{ NodeLValue{ memoryField.first, memoryField.second } } :
			NodeCell{ NodeRValue{ std::move(*memoryField.first), memoryField.second } };
	}
	
	auto& scriptZone = std::get<ExecutionContext>(variant);
	auto& memoryScript = scriptZone.program().currentMemory().down();
	auto memoryField = (memoryScript)(key);
	return isLvalue() ? NodeCell{ NodeLValue{ memoryField.first, memoryField.second } } :
		NodeCell{ NodeRValue{ std::move(*memoryField.first), memoryField.second } };
}

ska::NodeCell::NodeCell(NodeRValue rvalue) :
	m_memory(rvalue.memory),
	m_variant(std::move(rvalue.object)) {

}

ska::NodeCell::NodeCell(NodeLValue lvalue) :
	m_memory(lvalue.memory),
	m_variant(lvalue.object) {
	assert(m_memory != nullptr && "lvalue not provided (null)");
}

ska::NodeCell::NodeCell(NodeValue rvalue, MemoryTablePtr rvalueMemory) :
	m_variant(std::move(rvalue)),
	m_memory(rvalueMemory) {
}

ska::NodeLValue ska::NodeCell::asLvalue() {
	assert(isLvalue() && "Must be an lvalue");
	return { std::get<NodeValue*>(m_variant), m_memory };
}

ska::NodeRValue ska::NodeCell::asRvalue() {
	if (isLvalue()) {
		auto newValue = std::get<NodeValue*>(m_variant)->clone();
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
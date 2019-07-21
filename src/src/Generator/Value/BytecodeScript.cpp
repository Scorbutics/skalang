#include "BytecodeScript.h"
#include "Interpreter/Value/Script.h"

const ska::BytecodeCell& ska::BytecodeScript::reg() const {
	return m_register;
}

ska::BytecodeCell ska::BytecodeScript::stealReg() {
	auto result = std::move(m_register);
	m_register = {};
	return result;
}

void ska::BytecodeScript::setReg(BytecodeCell regist) {
	m_register = std::move(regist);
}

void ska::BytecodeScript::setGroup(std::string group) {
	if (group.empty()) {				
		m_group = " ";
		m_groupsSubVar[m_group] = 0;
	} else {
		auto ss = std::stringstream{};
		if (!m_group.empty()) {
			ss << m_group << " ";
		}
		ss << std::move(group);
		m_group = ss.str().empty() ? " " : ss.str();
		if (m_groupsSubVar.find(m_group) == m_groupsSubVar.end()) {
			m_groupsSubVar[m_group] = 0;
		}
	}
}

ska::BytecodeScript::BytecodeScript(Script& script) :
	m_script(script.handle()) {
}

std::string ska::BytecodeScript::newGroup(BytecodeCellGroup cellGroup) {
	auto groupName = nextGroupName();
	m_groupsSymbolTable[groupName] = std::move(cellGroup);
	return groupName;
}

std::string ska::BytecodeScript::nextGroupName() {
	auto ss = std::stringstream{};
	ss << m_groupsSubVar[m_group]++;
	return m_group + ss.str();
}

std::string ska::BytecodeScript::currentGroupName() {
	auto ss = std::stringstream{};
	ss << m_groupsSubVar[m_group];
	return m_group + ss.str();
}

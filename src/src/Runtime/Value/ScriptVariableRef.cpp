#include "ScriptVariableRef.h"

std::ostream& ska::operator<<(std::ostream& stream, const ScriptVariableRef& var) {
	stream << var.variable;
	return stream;
}

bool ska::operator==(const ScriptVariableRef& lhs, const ScriptVariableRef& rhs) {
	return lhs.script == rhs.script && lhs.variable == rhs.variable;
}

bool ska::operator==(const VariableRef& lhs, const VariableRef& rhs) {
	return lhs.variable == rhs.variable;
}

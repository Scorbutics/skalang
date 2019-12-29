#include "BytecodeSymbolInfo.h"

std::ostream& ska::bytecode::operator<<(std::ostream& stream, const SymbolInfo& symbol) {
	if(symbol.name != nullptr) {
		stream << *symbol.name;
	}
	stream << " (script_id|var_id:value) [";
	if(symbol.references != nullptr) {
		for(const auto& [key, value] : *symbol.references) {
			stream << " " << key.script << "|" << key.variable << ":" << value << ", ";
		}
	}
	stream << " ]";

	return stream;
}

std::ostream& ska::bytecode::operator<<(std::ostream& stream, const SymbolInfoPack& symbols) {
	for(const auto& c : symbols) {
		stream << c << " ";
	}
	return stream;
}

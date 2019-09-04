#include "BytecodeSymbolInfo.h"

std::ostream& ska::bytecode::operator<<(std::ostream& stream, const SymbolInfo& symbol) {
	if(symbol.name != nullptr) {
		stream << *symbol.name;
	}
	return stream;
}

std::ostream& ska::bytecode::operator<<(std::ostream& stream, const SymbolInfoPack& symbols) {
	for(const auto& c : symbols) {
		if(c.name != nullptr && !c.name->empty()) {
			stream << c << " ";
		}
		stream << " [";
		if(c.references != nullptr) {
			for(const auto& [key, value] : *c.references) {
				stream << " " << key << " : " << value;
			}
		}
		stream << " ]";
	}
	return stream;
}

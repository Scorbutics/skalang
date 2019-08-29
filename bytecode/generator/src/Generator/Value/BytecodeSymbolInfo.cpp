#include "BytecodeSymbolInfo.h"

std::ostream& ska::bytecode::operator<<(std::ostream& stream, const SymbolInfo& symbol) {
	stream << symbol.name;
	return stream;
}

std::ostream& ska::bytecode::operator<<(std::ostream& stream, const SymbolInfoPack& symbols) {
	for(const auto& c : symbols) {
		if(!c.name.empty()) {
			stream << c << " ";
		}
		if(c.references != nullptr) {
			stream << " [";
			for(const auto& [key, value] : *c.references) {
				stream << " " << key << " : " << value;
			}
			stream << " ]";
		}
	}
	return stream;
}

#include "BytecodeSymbolInfo.h"

std::ostream& ska::bytecode::operator<<(std::ostream& stream, const SymbolInfo& symbol) {
	if(symbol.name != nullptr) {
		stream << *symbol.name;
	}
	stream << " [";
	if(symbol.references != nullptr) {
		for(const auto& [key, value] : *symbol.references) {
			stream << " " << key << " : " << value;
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

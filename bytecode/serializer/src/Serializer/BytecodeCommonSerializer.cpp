#include "BytecodeCommonSerializer.h"

void ska::bytecode::CommonSerializer::write(std::stringstream& buffer, std::size_t value) {
	buffer.write(reinterpret_cast<const char*>(&value), sizeof(uint32_t));
}

void ska::bytecode::CommonSerializer::write(std::stringstream& buffer, std::string value) {

}

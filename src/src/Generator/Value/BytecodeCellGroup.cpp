#include "Config/LoggerConfigLang.h"
#include "BytecodeCellGroup.h"

std::ostream& ska::operator<<(std::ostream& stream, const ska::BytecodeCellGroup& group) {
		for(const auto& c : group) {
			stream << c << " ";
		}
}

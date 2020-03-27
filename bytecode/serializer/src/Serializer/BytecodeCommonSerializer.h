#pragma once
#include <sstream>
#include <cstddef>

namespace ska {
	namespace bytecode {
		class CommonSerializer {
		public:
			CommonSerializer() = default;
			~CommonSerializer() = default;

			static void write(std::stringstream& buffer, std::size_t value);
			static void write(std::stringstream& buffer, std::string value);
		};
	}
}
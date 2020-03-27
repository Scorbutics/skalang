#pragma once

#include <sstream>
#include <string>
#include <unordered_map>

namespace ska {
	namespace bytecode {
		struct Operand;
		class ScriptCache;
		class OperandSerializer {
		public:
			OperandSerializer() = default;
			~OperandSerializer() = default;

			static void write(const ScriptCache& cache, std::stringstream& buffer, std::unordered_map<std::string, std::size_t>& natives, const Operand& value);

		private:
			static const std::string scriptName(const ScriptCache& cache, std::size_t id);
		};
	}
}
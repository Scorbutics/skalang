#pragma once
#include <string>
#include <sstream>
#include <optional>
#include <unordered_map>

#include "Base/Values/MovableNonCopyable.h"
#include "Interpreter/Value/Script.h"
#include "Generator/Value/BytecodeValue.h"

namespace ska {
	namespace bytecode {
		class Script :
			public MovableNonCopyable {
		public:
			Script(ska::Script& script);
			
			Script(Script&&) = default;
			Script& operator=(Script&&) = default;

			ska::Script program() { return ska::Script{ *m_script }; }

			Register queryNextRegister(Type type) {
				auto ss = std::stringstream{};
				ss << "R" << m_register++;
				return { ss.str(), std::move(type) };
			}

		private:
			std::size_t m_register = 0;
			ska::ScriptHandle* m_script{};
		};
	}
}

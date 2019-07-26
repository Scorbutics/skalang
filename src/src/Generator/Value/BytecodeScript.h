#pragma once
#include <string>
#include <optional>
#include <unordered_map>

#include "Base/Values/MovableNonCopyable.h"
#include "Interpreter/Value/Script.h"

namespace ska {
	namespace bytecode {
		class Script :
			public MovableNonCopyable {
		public:
			Script(ska::Script& script);

			Script(Script&&) = default;
			Script& operator=(Script&&) = default;

			ska::Script program() { return ska::Script{ *m_script }; }

		private:
			//Value m_register;

			ska::ScriptHandle* m_script{};
		};
	}
}

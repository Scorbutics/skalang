#pragma once

namespace ska {
	namespace bytecode {
		struct VariableRef {			
			std::size_t variable = 0;
		};

		struct ScriptVariableRef {
			std::size_t variable = 0;
			std::size_t script = 0;
		};
	}
}

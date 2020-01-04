#pragma once
#include <memory>

namespace ska {
	class Script;
	//using ScriptPtr = std::unique_ptr<Script>;

	struct ScriptPtr : std::unique_ptr<Script> {
    	using std::unique_ptr<Script>::unique_ptr;
    	ScriptPtr(std::unique_ptr<Script> s);
    	ScriptPtr(ScriptPtr&& s) noexcept = default;
    	ScriptPtr() = default;
    	

    	~ScriptPtr(); // Implement (empty body) elsewhere
	};
}

#pragma once
#include <unordered_map>
#include <sstream>

namespace ska {
	class Symbol;
	struct Type;
	class SerializerOutput;

	class ScriptTypeSerializer {
	public:
		ScriptTypeSerializer() = default;
		virtual ~ScriptTypeSerializer() = default;
		
		virtual void write(SerializerOutput& output, const Symbol* symbol, const Type& type) = 0;
	};
}
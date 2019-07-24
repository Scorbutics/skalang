#pragma once
#include <string>
#include <optional>
#include <unordered_map>

#include "Generator/Value/BytecodeCellGroup.h"
#include "Base/Values/MovableNonCopyable.h"
#include "Interpreter/Value/Script.h"

namespace ska {
	class BytecodeScript : 
		public MovableNonCopyable {
	public:
		BytecodeScript(Script& script);

		BytecodeScript(BytecodeScript&&) = default;
		BytecodeScript& operator=(BytecodeScript&&) = default;

		const BytecodeCell& reg() const;
		BytecodeCell stealReg();
		void setReg(BytecodeCell regist);
		void setGroup(std::string group);

		std::optional<ska::BytecodeCell> package(BytecodeCellGroup cellGroup);
		
		Script program() {
			return Script{ *m_script };
		}

	private:
		std::string nextGroupName();
		std::string currentGroupName();
		
		BytecodeCell m_register;
		std::string m_group = " ";
		
		std::unordered_map<std::string, std::size_t> m_groupsSubVar;
		std::unordered_map<std::string, BytecodeCellGroup> m_groupsSymbolTable;

		ScriptHandle* m_script{};
	};
}

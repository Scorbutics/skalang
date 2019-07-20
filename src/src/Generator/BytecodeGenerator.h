#pragma once
#include <sstream>
#include <memory>
#include <vector>
#include "GeneratorOperatorUnit.h"
#include "NodeValue/Operator.h"
#include "NodeValue/ASTNodePtr.h"
#include "BytecodeGenerationContext.h"

namespace ska {
    struct ReservedKeywordsPool;
	class Script;
	class TypeCrosser;

	class BytecodeGenerator {
		using OperatorGenerator = std::vector<std::unique_ptr<GeneratorOperatorUnit>>;
	public:
		BytecodeGenerator(const ReservedKeywordsPool& reserved, const TypeCrosser& typeCrosser);
		~BytecodeGenerator() = default;

		BytecodeCellGroup generate(BytecodeGenerationContext node);
		BytecodeCellGroup generate(Script& script);

		////////////
		const BytecodeCell& reg() const {
			return m_register;
		}

		BytecodeCell stealReg() {
			auto result = std::move(m_register);
			m_register = {};
			return result;
		}

		void setReg(BytecodeCell regist) {
			m_register = std::move(regist);
		}

		void setGroup(std::string group) {
			if (group.empty()) {				
				m_group = " ";
				m_groupsSubVar[m_group] = 0;
			} else {
				auto ss = std::stringstream{};
				if (!m_group.empty()) {
					ss << m_group << " ";
				}
				ss << std::move(group);
				m_group = ss.str().empty() ? " " : ss.str();
				if (m_groupsSubVar.find(m_group) == m_groupsSubVar.end()) {
					m_groupsSubVar[m_group] = 0;
				}
			}
			
		}

		std::string newGroup(BytecodeCellGroup cellGroup) {
			auto groupName = nextGroupName();
			m_groupsSymbolTable[groupName] = std::move(cellGroup);
			return groupName;
		}

	private:
		std::string nextGroupName() {
			auto ss = std::stringstream{};
			ss << m_groupsSubVar[m_group]++;
			return m_group + ss.str();
		}

		std::string currentGroupName() {
			auto ss = std::stringstream{};
			ss << m_groupsSubVar[m_group];
			return m_group + ss.str();
		}

		OperatorGenerator build();
		OperatorGenerator m_operatorGenerator;
		const TypeCrosser& m_typeCrosser;
		
		/////////////////
		//Generation steps variables
		BytecodeCell m_register;
		std::string m_group = " ";
		
		std::unordered_map<std::string, std::size_t> m_groupsSubVar;
		std::unordered_map<std::string, BytecodeCellGroup> m_groupsSymbolTable;
	};
}

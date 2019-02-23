#pragma once
#include <vector>

#include "Container/sorted_observable.h"

#include "NodeValue/Token.h"
#include "Service/ExpressionParser.h"

#include "Event/ForTokenEvent.h"
#include "Event/IfElseTokenEvent.h"
#include "Event/VarTokenEvent.h"
#include "Event/BlockTokenEvent.h"
#include "Event/FunctionTokenEvent.h"
#include "Event/ExpressionTokenEvent.h"
#include "Event/ReturnTokenEvent.h"
#include "Event/ArrayTokenEvent.h"
#include "Event/ImportTokenEvent.h"
#include "Event/BridgeTokenEvent.h"

#include "Matcher/MatcherBlock.h"
#include "Matcher/MatcherFor.h"
#include "Matcher/MatcherIfElse.h"
#include "Matcher/MatcherVar.h"
#include "Matcher/MatcherReturn.h"

namespace ska {
	struct ReservedKeywordsPool;
	class Script;
	class ExpressionParser;
	struct ScriptHandle;

	class StatementParser :
	    public observable_priority_queue<ForTokenEvent>,
        public observable_priority_queue<IfElseTokenEvent>,
		public observable_priority_queue<VarTokenEvent>,
		public observable_priority_queue<FunctionTokenEvent>,
		public observable_priority_queue<BlockTokenEvent>,
		public observable_priority_queue<ExpressionTokenEvent>,
		public observable_priority_queue<ReturnTokenEvent>,
		public observable_priority_queue<ArrayTokenEvent>,
		public observable_priority_queue<ImportTokenEvent>,
		public observable_priority_queue<BridgeTokenEvent> {

		using ASTNodePtr = std::unique_ptr<ska::ASTNode>;
		friend class Script;
		
		using ScriptHandlePtr = std::unique_ptr<ScriptHandle>;
	public:
		StatementParser(const ReservedKeywordsPool& reservedKeywordsPool);
		Script subParse(std::unordered_map<std::string, ScriptHandlePtr>& scriptCache, const std::string& name, std::ifstream& file);

	private:
		ASTNodePtr parse(Script& input);

		ASTNodePtr statement(Script& input);
        ASTNodePtr optstatement(Script& input, const Token& mustNotBe = Token{});

		ASTNodePtr expr(Script& input);
		ASTNodePtr optexpr(Script& input, const Token& mustNotBe = Token{});

		ASTNodePtr matchExpressionStatement(Script& input);
		ASTNodePtr matchReservedKeyword(Script& input, const std::size_t keywordIndex);
		static void error(const std::string& message);

		const ReservedKeywordsPool& m_reservedKeywordsPool;
		ExpressionParser m_expressionParser;
		
		MatcherBlock m_matcherBlock;
		MatcherFor m_matcherFor;
		MatcherVar m_matcherVar;
		MatcherIfElse m_matcherIfElse;
		MatcherReturn m_matcherReturn;
		MatcherImport m_matcherImport;
	};
}

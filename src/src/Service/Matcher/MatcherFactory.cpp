#include "Config/LoggerConfigLang.h"
#include "MatcherFactory.h"

#include "NodeValue/AST.h"
#include "Service/StatementParser.h"
#include "Service/TokenReader.h"
#include "Service/ReservedKeywordsPool.h"
#include "Service/ASTFactory.h"
#include "NodeValue/ScriptAST.h"
#include "Event/ReturnTokenEvent.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::MatcherFactory)

static constexpr const auto* ThisPrivateName = "this.private";
static constexpr const auto* ThisPrivateMemberName = "this.private.member";
static constexpr const auto* ThisPrivateFactoryName = "this.private.fcty";

ska::ASTNodePtr ska::MatcherFactory::buildThisObject(ScriptAST& input) {
	auto* privateThisObject = input.symbols()[ThisPrivateName];
	auto privateThisObjectNode = ASTFactory::MakeNode<Operator::PARAMETER_DECLARATION>(std::move(Token{ ThisPrivateMemberName, TokenType::IDENTIFIER, {} }), ASTFactory::MakeEmptyNode());

	if (privateThisObject != nullptr) {
		(*privateThisObjectNode)[0].updateType(privateThisObject->type());
		(*privateThisObjectNode)[0].linkSymbol(*privateThisObject);
	} else {
		auto* privateThisFcty = input.symbols()[ThisPrivateFactoryName];
		(*privateThisObjectNode)[0].updateType(Type::MakeCustom<ExpressionType::OBJECT>(privateThisFcty));
	}

	auto event = VarTokenEvent::MakeParameter(*privateThisObjectNode, (*privateThisObjectNode)[0], input);
	m_parser.observable_priority_queue<VarTokenEvent>::notifyObservers(event);

	return privateThisObjectNode;
}

bool ska::MatcherFactory::isFunctionMember(const Symbol& symbol) const {
	return symbol.type() == ExpressionType::FUNCTION && symbol.type()[0].name() == ThisPrivateFactoryName;
}

ska::ASTNodePtr ska::MatcherFactory::matchPrivateObject(ScriptAST& input, const Token& privateFactoryName) {
	auto functionCall = ASTFactory::MakeNode<Operator::FUNCTION_CALL>(ASTFactory::MakeLogicalNode(privateFactoryName));

	auto eventCallPrivateObject = FunctionTokenEvent{ *functionCall, FunctionTokenEventType::CALL, input };
	m_parser.observable_priority_queue<FunctionTokenEvent>::notifyObservers(eventCallPrivateObject);

	auto object = ASTFactory::MakeNode<Operator::VARIABLE_AFFECTATION>(Token{ ThisPrivateName, TokenType::IDENTIFIER, privateFactoryName.position() }, std::move(functionCall));
	auto event = VarTokenEvent::template Make<VarTokenEventType::VARIABLE_AFFECTATION>(*object, input);
	m_parser.observable_priority_queue<VarTokenEvent>::notifyObservers(event);
	return object;
}

std::vector<ska::ASTNodePtr> ska::MatcherFactory::matchDeclarationBody(ScriptAST& input, const Token& endToken) {
    auto statements = std::vector<ASTNodePtr>{};
	while (!input.reader().expect(endToken)) {
		auto optionalStatement = input.statement(m_parser);
		if (optionalStatement != nullptr && !optionalStatement->logicalEmpty()) {
			statements.push_back(std::move(optionalStatement));
		}
	}
    return statements;
}

ska::ASTNodePtr ska::MatcherFactory::matchPrivateFactory(ScriptAST& input, const ASTNode& functionPrototype) {
	return nullptr;
}

ska::ASTNodePtr ska::MatcherFactory::matchDeclaration(ScriptAST& input, const Token& functionName, std::deque<ASTNodePtr> parameters, ASTNodePtr returnType) {
	input.pushContext({ParsingContextType::FACTORY_DECLARATION, functionName});
	input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_BEGIN>());

	SLOG(ska::LogLevel::Debug) << "factory matching declaration and parameters part";
	// Match the function name declaration
	// e.g. function(i: int): var
	for (auto& parameter : parameters) {
		auto event = VarTokenEvent::MakeParameter(*parameter, (*parameter)[0], input);
		m_parser.observable_priority_queue<VarTokenEvent>::notifyObservers(event);
	}
	parameters.push_back(std::move(returnType));
	auto functionPrototypeNode = ASTFactory::MakeNode<Operator::FUNCTION_PROTOTYPE_DECLARATION>(functionName, std::move(parameters));

	auto functionEvent = VarTokenEvent::MakeFunction(*functionPrototypeNode, input);
	// Declares also the function as a variable
	m_parser.observable_priority_queue<VarTokenEvent>::notifyObservers(functionEvent);

	// Match the factory function body until the "return" token
	// e.g. any statement following "function(i: int): var"
	SLOG(ska::LogLevel::Debug) << "factory matching body part";
	auto bodyNodes = matchDeclarationBody(input, m_reservedKeywordsPool.pattern<TokenGrammar::RETURN>());

	// Match the return part of the factory, where the object is built
	// e.g. return { toto = 1 }
	SLOG(ska::LogLevel::Debug) << "factory matching generated object part";
	auto returnNode = m_matcherReturn.match(input, true);

	auto fullBodyNode = ASTFactory::MakeNode<Operator::BLOCK>(std::move(bodyNodes));

	auto factoryPrototypeNode = ASTFactory::MakeNode<Operator::FUNCTION_DECLARATION>(functionName, std::move(functionPrototypeNode), std::move(fullBodyNode));

	auto prototypeFactoryEvent = FunctionTokenEvent{ *factoryPrototypeNode, FunctionTokenEventType::FACTORY_DECLARATION_STATEMENT, input, functionName.name() };
	m_parser.observable_priority_queue<FunctionTokenEvent>::notifyObservers(prototypeFactoryEvent);

	input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_END>());
	input.popContext();
	return factoryPrototypeNode;
}

ska::ASTNodePtr ska::MatcherFactory::matchPrivateFieldUse(ScriptAST& input, ASTNodePtr varNode) {
	const auto thisPrivateMember = ThisPrivateMemberName;
	const auto* thisPrivateMemberSymbol = input.symbols()[thisPrivateMember];
	if (thisPrivateMemberSymbol == nullptr || (*thisPrivateMemberSymbol)(varNode->name()) == nullptr) {
		return varNode;
	}
	auto privateThisObjectNode = ASTFactory::MakeLogicalNode(Token{thisPrivateMember , TokenType::IDENTIFIER, input.reader().actual().position() });
	auto finalNode = ASTFactory::MakeNode<Operator::FIELD_ACCESS>(std::move(privateThisObjectNode), std::move(varNode));
	return finalNode;
}

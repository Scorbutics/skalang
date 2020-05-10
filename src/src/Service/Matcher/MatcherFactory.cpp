#include "Config/LoggerConfigLang.h"
#include "MatcherFactory.h"

#include "NodeValue/AST.h"
#include "Service/StatementParser.h"
#include "Service/TokenReader.h"
#include "Service/ReservedKeywordsPool.h"
#include "Service/ASTFactory.h"
#include "NodeValue/ScriptAST.h"
#include "Event/ReturnTokenEvent.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::MatcherFactory)

static constexpr const auto* ThisPrivateName = "this.private";
static constexpr const auto* ThisName = "this.";
static constexpr const auto* ThisPrivateFactoryName = "this.private.fcty";

ska::ASTNodePtr ska::MatcherFactory::buildThisObject(ScriptAST& input) {
	auto* privateThisObject = input.symbols()[ThisPrivateName];
	auto privateThisObjectNode = ASTFactory::MakeNode<Operator::PARAMETER_DECLARATION>(std::move(Token{ ThisPrivateName + std::string{".member"}, TokenType::IDENTIFIER, {} }), ASTFactory::MakeEmptyNode());
	(*privateThisObjectNode)[0].updateType(privateThisObject->type());
	(*privateThisObjectNode)[0].linkSymbol(*privateThisObject);

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

ska::ASTNodePtr ska::MatcherFactory::matchPrivateFactory(ScriptAST& input, std::deque<ASTNodePtr> parameters) {
	auto functionPrivateObjectToken = Token{ ThisPrivateFactoryName, TokenType::IDENTIFIER, input.reader().actual().position() };

	auto emptyNode = ASTFactory::MakeEmptyNode();
	auto startEvent = FunctionTokenEvent{ *emptyNode, FunctionTokenEventType::DECLARATION_NAME, input, functionPrivateObjectToken.name() };
	m_parser.observable_priority_queue<FunctionTokenEvent>::notifyObservers(startEvent);

	auto returnTypeNode = m_matcherType.match(Type::MakeCustom<ExpressionType::OBJECT>(nullptr));
	auto prototypeNode = ASTFactory::MakeNode<Operator::FUNCTION_PROTOTYPE_DECLARATION>(functionPrivateObjectToken, std::move(returnTypeNode));

	auto functionEvent = VarTokenEvent::MakeFunction(*prototypeNode, input);
	m_parser.observable_priority_queue<VarTokenEvent>::notifyObservers(functionEvent);

	auto privateReturnEventStart = ReturnTokenEvent{input};
    m_parser.observable_priority_queue<ReturnTokenEvent>::notifyObservers(privateReturnEventStart);

	for (auto& parameter : parameters) {
		auto event = VarTokenEvent::Make<VarTokenEventType::VARIABLE_AFFECTATION>(*parameter, input);
		m_parser.observable_priority_queue<VarTokenEvent>::notifyObservers(event);
	}
	
	auto tmpFunctionBodyPrivateNodeList = matchDeclarationBody(input, m_reservedKeywordsPool.pattern<TokenGrammar::RETURN>());
	auto privateNodes = std::move(parameters);
	std::move(tmpFunctionBodyPrivateNodeList.begin(), tmpFunctionBodyPrivateNodeList.end(), std::back_inserter(privateNodes));

	auto privateNodeObj = ASTFactory::MakeNode<Operator::RETURN>(ASTFactory::MakeNode<Operator::USER_DEFINED_OBJECT>(std::move(privateNodes)));
	auto privateReturnEvent = ReturnTokenEvent::template Make<ReturnTokenEventType::OBJECT>(*privateNodeObj, input);
    m_parser.observable_priority_queue<ReturnTokenEvent>::notifyObservers(privateReturnEvent);
	
	auto functionPrivateObject = ASTFactory::MakeNode<Operator::FUNCTION_DECLARATION>(functionPrivateObjectToken, std::move(prototypeNode), ASTFactory::MakeNode<Operator::BLOCK>(std::move(privateNodeObj)));
	
	auto statementEvent = FunctionTokenEvent{ *functionPrivateObject, FunctionTokenEventType::DECLARATION_STATEMENT, input, functionPrivateObject->name() };
	m_parser.observable_priority_queue<FunctionTokenEvent>::notifyObservers(statementEvent);

	return functionPrivateObject;
}

ska::ASTNodePtr ska::MatcherFactory::matchPublicObject(ScriptAST& input, const Token& functionName) {
	auto publicStartReturnTokenEvent = ReturnTokenEvent{ input };
	m_parser.observable_priority_queue<ReturnTokenEvent>::notifyObservers(publicStartReturnTokenEvent);

	auto publicFields = std::vector<ASTNodePtr>{};

	// The "this.private" object is the first element (but hidden) of public object
	auto privateThisObjectNode = matchPrivateObject(input, Token{ ThisPrivateFactoryName, TokenType::IDENTIFIER, input.reader().actual().position() });
	publicFields.push_back(std::move(privateThisObjectNode));
	auto event = VarTokenEvent::template Make<VarTokenEventType::VARIABLE_AFFECTATION>(*publicFields.back(), input);
	m_parser.observable_priority_queue<VarTokenEvent>::notifyObservers(event);

	auto publicReturnNode = m_matcherReturn.match(input, true);
	for (auto& field : (*publicReturnNode)[0]) {
		publicFields.push_back(std::move(field));
	}

	/*
	for (auto& fields : (*publicNodes[0])[0]) {
		if (fields->symbol() != nullptr) {
			auto* functionNameSymbol = input.symbols()[functionName.name()];
			assert(functionNameSymbol != nullptr);
			fields->symbol()->makeField(*functionNameSymbol);
		}
	}
	*/


	auto publicNodeObj = ASTFactory::MakeNode<Operator::RETURN>(ASTFactory::MakeNode<Operator::USER_DEFINED_OBJECT>(std::move(publicFields)));
	auto publicNodeObjReturnEvent = ReturnTokenEvent::template Make<ReturnTokenEventType::OBJECT>(*publicNodeObj, input);
	m_parser.observable_priority_queue<ReturnTokenEvent>::notifyObservers(publicNodeObjReturnEvent);
	return publicNodeObj;
}

ska::ASTNodePtr ska::MatcherFactory::matchDeclaration(ScriptAST& input, const Token& functionName, std::deque<ASTNodePtr> parameters, ASTNodePtr returnType) {
	input.pushContext({ParsingContextType::FACTORY_DECLARATION, functionName});
	input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_BEGIN>());

	SLOG(ska::LogLevel::Debug) << "factory matching private object part";

	auto blockParameters = std::vector<ASTNodePtr>{};
	for (const auto& parameter : parameters) {
		blockParameters.push_back(ASTFactory::MakeLogicalNode(Token{ parameter->name(), TokenType::IDENTIFIER, parameter->positionInScript() }));
	}

	auto privateFunctionFactory = matchPrivateFactory(input, std::move(parameters));

	for (auto& parameter : blockParameters) {
		auto* param = (*privateFunctionFactory->symbol())(parameter->name());
		if (param != nullptr) {
			parameter->linkSymbol(*param);
		}
	}

	SLOG(ska::LogLevel::Debug) << "factory matching public object part";
		
	auto factoryPrototypeNode = ASTFactory::MakeNode<Operator::FACTORY_PROTOTYPE_DECLARATION>(functionName, std::move(privateFunctionFactory), ASTFactory::MakeNode<Operator::BLOCK>(std::move(blockParameters)), std::move(returnType));

	auto prototypeFactoryEvent = FunctionTokenEvent{ *factoryPrototypeNode, FunctionTokenEventType::FACTORY_PROTOTYPE, input, functionName.name() };
	m_parser.observable_priority_queue<FunctionTokenEvent>::notifyObservers(prototypeFactoryEvent);

	auto publicThisObjectReturnNode = matchPublicObject(input, functionName);

	SLOG(ska::LogLevel::Debug) << "factory synthetizing node";
	input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_END>());
	auto factoryDeclarationNode = ASTFactory::MakeNode<Operator::FUNCTION_DECLARATION>(functionName, std::move(factoryPrototypeNode), std::move(publicThisObjectReturnNode));

	auto statementEvent = FunctionTokenEvent{ *factoryDeclarationNode, FunctionTokenEventType::FACTORY_DECLARATION_STATEMENT, input, functionName.name() };
	m_parser.observable_priority_queue<FunctionTokenEvent>::notifyObservers(statementEvent);

	input.popContext();
	return factoryDeclarationNode;
}

ska::ASTNodePtr ska::MatcherFactory::matchPrivateFieldUse(ScriptAST& input, ASTNodePtr varNode) {
	const auto thisPrivateMember = ThisPrivateName + std::string{".member"};
	if ((*input.symbols()[thisPrivateMember])(varNode->name()) == nullptr) {
		return varNode;
	}
	auto privateThisObjectNode = ASTFactory::MakeLogicalNode(Token{thisPrivateMember , TokenType::IDENTIFIER, input.reader().actual().position() });
	auto finalNode = ASTFactory::MakeNode<Operator::FIELD_ACCESS>(std::move(privateThisObjectNode), std::move(varNode));
	return finalNode;
}

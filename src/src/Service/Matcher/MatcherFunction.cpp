#include "Config/LoggerConfigLang.h"
#include "MatcherFunction.h"

#include "NodeValue/AST.h"
#include "Service/StatementParser.h"
#include "Service/TokenReader.h"
#include "Service/ReservedKeywordsPool.h"
#include "Service/ASTFactory.h"
#include "NodeValue/ScriptAST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::MatcherFunction)

ska::ASTNodePtr ska::MatcherFunction::matchDeclaration(ScriptAST& input) {
	SLOG(ska::LogLevel::Debug) << "function declaration";
	input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::FUNCTION>());

    //With this grammar, no other way than reading previously to retrieve the function name.
    const auto functionName = input.reader().readPrevious(3); 

	auto emptyNode = ASTFactory::MakeEmptyNode();
	auto startEvent = FunctionTokenEvent{ *emptyNode, FunctionTokenEventType::DECLARATION_NAME, input, functionName.name() };
	m_parser.observable_priority_queue<FunctionTokenEvent>::notifyObservers(startEvent);

	auto declNode = fillDeclarationParameters(input);
	declNode.push_back(matchDeclarationReturnType(input));

	auto prototypeNode = ASTFactory::MakeNode<Operator::FUNCTION_PROTOTYPE_DECLARATION>(functionName, std::move(declNode));

	auto functionEvent = VarTokenEvent::MakeFunction(*prototypeNode, input);
	m_parser.observable_priority_queue<VarTokenEvent>::notifyObservers(functionEvent);

    SLOG(ska::LogLevel::Debug) << "reading function body";
	auto functionBodyNode = matchDeclarationBody(input);
	SLOG(ska::LogLevel::Debug) << "function read.";
	
	auto functionDeclarationNode = ASTFactory::MakeNode<Operator::FUNCTION_DECLARATION>(functionName, std::move(prototypeNode), std::move(functionBodyNode));
	
	auto statementEvent = FunctionTokenEvent {*functionDeclarationNode, FunctionTokenEventType::DECLARATION_STATEMENT, input, functionName.name() };
	m_parser.observable_priority_queue<FunctionTokenEvent>::notifyObservers(statementEvent);

	return functionDeclarationNode;
}

ska::ASTNodePtr ska::MatcherFunction::matchCall(ScriptAST& input, ASTNodePtr identifierFunctionName) {
	input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_BEGIN>());

	auto functionCallNodeContent = std::vector<ASTNodePtr>{};

	functionCallNodeContent.push_back(std::move(identifierFunctionName));
	
	const auto endParametersToken = m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>();
	const auto endStatementToken = m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>();
	while (!input.reader().expect(endParametersToken)) {

		auto expressionOpt = input.expr(m_parser);
		if (expressionOpt != nullptr) {
			SLOG(ska::LogLevel::Debug) << "Expression not null";
			functionCallNodeContent.push_back(std::move(expressionOpt));
		} else {
			SLOG(ska::LogLevel::Debug) << "Expression null";
			if (input.reader().expect(endStatementToken)) {
				break;
			}
		}

        const auto commaToken = m_reservedKeywordsPool.pattern<TokenGrammar::ARGUMENT_DELIMITER>();
		if(input.reader().expect(commaToken)) {
			input.reader().match(commaToken);
		}
	}
	if (input.reader().expect(endParametersToken)) {
		input.reader().match(endParametersToken);
	}

	auto functionCallNode = ASTFactory::MakeNode<Operator::FUNCTION_CALL>(std::move(functionCallNodeContent));
	auto event = FunctionTokenEvent { *functionCallNode, FunctionTokenEventType::CALL, input };
	m_parser.observable_priority_queue<FunctionTokenEvent>::notifyObservers(event);
	return functionCallNode;
}


ska::ASTNodePtr ska::MatcherFunction::matchDeclarationParameter(ScriptAST& input) {
	const auto isRightParenthesis = input.reader().expect(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>());
	if(isRightParenthesis) {
		return nullptr;
	}
	const auto& id = input.reader().match(TokenType::IDENTIFIER);

	const auto& typeDelimiterToken = m_reservedKeywordsPool.pattern<TokenGrammar::TYPE_DELIMITER>();
    input.reader().match(typeDelimiterToken);
	
	auto typeNameNode = m_matcherType.match(input.reader());
		
	SLOG(ska::LogLevel::Debug) << id.name();
	auto node = ASTFactory::MakeNode<Operator::PARAMETER_DECLARATION>(id, std::move(typeNameNode));
	auto event = VarTokenEvent::MakeParameter(*node, (*node)[0], input);
	m_parser.observable_priority_queue<VarTokenEvent>::notifyObservers(event);
	return node;
}

std::vector<ska::ASTNodePtr> ska::MatcherFunction::fillDeclarationParameters(ScriptAST& input) {
	input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_BEGIN>());

	auto parameters = std::vector<ASTNodePtr>{};
	auto isRightParenthesis = input.reader().expect(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>());
	auto isComma = true;
	while (!isRightParenthesis && isComma) {
		if (!input.reader().expect(TokenType::SYMBOL)) {
			SLOG(ska::LogLevel::Debug) << "parameter detected, reading identifier : ";
			auto parameterNode = matchDeclarationParameter(input);
			parameters.push_back(std::move(parameterNode));
			isComma = input.reader().expect(m_reservedKeywordsPool.pattern<TokenGrammar::ARGUMENT_DELIMITER>());
			if (isComma) {
				input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::ARGUMENT_DELIMITER>());
			}
		}
		isRightParenthesis = input.reader().expect(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>());
	}
	input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>());
	
	return parameters;
}

ska::ASTNodePtr ska::MatcherFunction::matchDeclarationReturnType(ScriptAST& input) {
	if (input.reader().expect(m_reservedKeywordsPool.pattern<TokenGrammar::TYPE_DELIMITER>())) {
		input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::TYPE_DELIMITER>());
		auto typeNode = m_matcherType.match(input.reader());
		SLOG(ska::LogLevel::Debug) << "function type detected : " << typeNode->name();
		return typeNode;
	} 

	SLOG(ska::LogLevel::Debug) << "void function detected";
	return ASTFactory::MakeLogicalNode(ska::Token{ "", ska::TokenType::IDENTIFIER, input.reader().actual().position() });
}

ska::ASTNodePtr ska::MatcherFunction::matchDeclarationBody(ScriptAST& input) {
	input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_BEGIN>());

	auto statements = std::vector<ASTNodePtr>{};
	while (!input.reader().expect(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_END>())) {
		auto optionalStatement = input.statement(m_parser);
		if (optionalStatement != nullptr && !optionalStatement->logicalEmpty()) {
			statements.push_back(std::move(optionalStatement));
		} else {
			break;
		}
	}
	input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_END>());

	return ASTFactory::MakeNode<Operator::BLOCK>(std::move(statements));
}

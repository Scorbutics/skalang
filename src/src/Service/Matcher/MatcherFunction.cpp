#include "Config/LoggerConfigLang.h"
#include "MatcherFunction.h"

#include "NodeValue/AST.h"
#include "Service/StatementParser.h"
#include "Service/TokenReader.h"
#include "Service/ReservedKeywordsPool.h"
#include "Service/ASTFactory.h"
#include "Service/Script.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::MatcherFunction)

ska::ASTNodePtr ska::MatcherFunction::matchDeclaration(Script& input) {
	SLOG(ska::LogLevel::Debug) << "function declaration";
	input.match(m_reservedKeywordsPool.pattern<TokenGrammar::FUNCTION>());

    //With this grammar, no other way than reading previously to retrieve the function name.
    const auto functionName = input.readPrevious(3); 

	auto emptyNode = ASTFactory::MakeEmptyNode();
	auto startEvent = FunctionTokenEvent{ *emptyNode, FunctionTokenEventType::DECLARATION_NAME, functionName.name() };
	m_parser.Observable<FunctionTokenEvent>::notifyObservers(startEvent);

	auto declNode = fillDeclarationParameters(input);
	declNode.push_back(matchDeclarationReturnType(input));

	auto prototypeNode = ASTFactory::MakeNode<Operator::FUNCTION_PROTOTYPE_DECLARATION>(functionName, std::move(declNode));

	auto functionEvent = VarTokenEvent::MakeFunction(*prototypeNode);
	m_parser.Observable<VarTokenEvent>::notifyObservers(functionEvent);

    SLOG(ska::LogLevel::Debug) << "reading function body";
	auto functionBodyNode = matchDeclarationBody(input);
	SLOG(ska::LogLevel::Debug) << "function read.";
	
	auto functionDeclarationNode = ASTFactory::MakeNode<Operator::FUNCTION_DECLARATION>(functionName, std::move(prototypeNode), std::move(functionBodyNode));
	
	auto statementEvent = FunctionTokenEvent {*functionDeclarationNode, FunctionTokenEventType::DECLARATION_STATEMENT, functionName.name() };
	m_parser.Observable<FunctionTokenEvent>::notifyObservers(statementEvent);

	return functionDeclarationNode;
}

ska::ASTNodePtr ska::MatcherFunction::matchCall(Script& input, ASTNodePtr identifierFunctionName) {
	input.match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_BEGIN>());

	auto functionCallNodeContent = std::vector<ASTNodePtr>{};

	functionCallNodeContent.push_back(std::move(identifierFunctionName));
	
	const auto endParametersToken = m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>();
	while (!input.expect(endParametersToken)) {

		auto expressionOpt = input.expr(m_parser);
		if (expressionOpt != nullptr) {
			SLOG(ska::LogLevel::Debug) << "Expression not null";
			functionCallNodeContent.push_back(std::move(expressionOpt));
		} else {
			SLOG(ska::LogLevel::Debug) << "Expression null";
		}

        const auto commaToken = m_reservedKeywordsPool.pattern<TokenGrammar::ARGUMENT_DELIMITER>();
		if(input.expect(commaToken)) {
			input.match(commaToken);
		}
	}
	input.match(endParametersToken);

	auto functionCallNode = ASTFactory::MakeNode<Operator::FUNCTION_CALL>(std::move(functionCallNodeContent));
	auto event = FunctionTokenEvent { *functionCallNode, FunctionTokenEventType::CALL };
	m_parser.Observable<FunctionTokenEvent>::notifyObservers(event);
	return functionCallNode;
}

ska::ASTNodePtr ska::MatcherFunction::matchDeclarationParameter(Script& input) {
	const auto isRightParenthesis = input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>());
	if(isRightParenthesis) {
		return nullptr;
	}
	const auto& id = input.match(TokenType::IDENTIFIER);
	SLOG(ska::LogLevel::Debug) << id.name();
	
    input.match(m_reservedKeywordsPool.pattern<TokenGrammar::TYPE_DELIMITER>());
    const auto typeToken = 
		input.expect(TokenType::RESERVED) ?
		input.match(TokenType::RESERVED) : input.match(TokenType::IDENTIFIER);
	const auto typeStr = typeToken.name();

	SLOG(ska::LogLevel::Debug) << "type is : " << typeStr;
	
	//handle arrays
	auto typeNode = ASTNodePtr{};
	if (input.expect(TokenType::ARRAY)) {
		auto arrayStartToken = input.match(TokenType::ARRAY);
		auto arrayEndToken = input.match(TokenType::ARRAY);
		if (std::get<std::string>(arrayStartToken.content()) != "[" ||
			std::get<std::string>(arrayEndToken.content()) != "]") {
			throw std::runtime_error("syntax error : only brackets [] are supported in a parameter declaration type");
		}

		typeNode = ASTFactory::MakeLogicalNode(typeToken, ASTFactory::MakeLogicalNode(Token{"", TokenType::ARRAY }));
	} else {
		typeNode = ASTFactory::MakeLogicalNode(typeToken);
	}

	auto node = ASTFactory::MakeNode<Operator::PARAMETER_DECLARATION>(id, std::move(typeNode));
	auto event = VarTokenEvent::MakeParameter(*node, (*node)[0]);
	m_parser.Observable<VarTokenEvent>::notifyObservers(event);
	return node;
}

std::vector<ska::ASTNodePtr> ska::MatcherFunction::fillDeclarationParameters(Script& input) {
	input.match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_BEGIN>());

	auto parameters = std::vector<ASTNodePtr>{};
	auto isRightParenthesis = input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>());
	auto isComma = true;
	while (!isRightParenthesis && isComma) {
		if (!input.expect(TokenType::SYMBOL)) {
			SLOG(ska::LogLevel::Debug) << "parameter detected, reading identifier : ";
			auto parameterNode = matchDeclarationParameter(input);
			parameters.push_back(std::move(parameterNode));
			isComma = input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::ARGUMENT_DELIMITER>());
			if (isComma) {
				input.match(m_reservedKeywordsPool.pattern<TokenGrammar::ARGUMENT_DELIMITER>());
			}
		}
		isRightParenthesis = input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>());
	}
	input.match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>());
	
	return parameters;
}

ska::ASTNodePtr ska::MatcherFunction::matchDeclarationReturnType(Script& input) {
	if (input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::TYPE_DELIMITER>())) {
		input.match(m_reservedKeywordsPool.pattern<TokenGrammar::TYPE_DELIMITER>());
		const auto type = input.match(TokenType::RESERVED);
		SLOG(ska::LogLevel::Debug) << "function type detected : " << type;
		return ASTFactory::MakeLogicalNode(type);
	} 

	SLOG(ska::LogLevel::Debug) << "void function detected";
	return ASTFactory::MakeLogicalNode(ska::Token{ "", ska::TokenType::IDENTIFIER });
}

ska::ASTNodePtr ska::MatcherFunction::matchDeclarationBody(Script& input) {
	input.match(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_BEGIN>());

	auto statements = std::vector<ASTNodePtr>{};
	while (!input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_END>())) {
		auto optionalStatement = input.statement(m_parser);
		if (!optionalStatement->logicalEmpty()) {
			statements.push_back(std::move(optionalStatement));
		} else {
			break;
		}
	}
	input.match(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_END>());

	return ASTFactory::MakeNode<Operator::BLOCK>(std::move(statements));
}

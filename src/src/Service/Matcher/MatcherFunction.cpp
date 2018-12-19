#include "Config/LoggerConfigLang.h"
#include "MatcherFunction.h"

#include "NodeValue/AST.h"
#include "Service/Parser.h"
#include "Service/TokenReader.h"
#include "Service/ReservedKeywordsPool.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::MatcherFunction)

ska::ASTNodePtr ska::MatcherFunction::matchDeclaration() {
	SLOG(ska::LogLevel::Debug) << "function declaration";
	m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::FUNCTION>());

    //With this grammar, no other way than reading previously to retrieve the function name.
    const auto functionName = m_input.readPrevious(3); 

	auto emptyNode = ASTNode::MakeEmptyNode();
	auto startEvent = FunctionTokenEvent{ *emptyNode, FunctionTokenEventType::DECLARATION_NAME, functionName.name() };
	m_parser.Observable<FunctionTokenEvent>::notifyObservers(startEvent);

	auto declNode = fillDeclarationParameters();
	declNode.push_back(matchDeclarationReturnType());

	auto prototypeNode = ASTNode::MakeNode<Operator::FUNCTION_PROTOTYPE_DECLARATION>(functionName, std::move(declNode));

	auto functionEvent = VarTokenEvent::MakeFunction(*prototypeNode);
	m_parser.Observable<VarTokenEvent>::notifyObservers(functionEvent);

    SLOG(ska::LogLevel::Debug) << "reading function body";
	auto functionBodyNode = matchDeclarationBody();
	SLOG(ska::LogLevel::Debug) << "function read.";
	
	auto functionDeclarationNode = ASTNode::MakeNode<Operator::FUNCTION_DECLARATION>(functionName, std::move(prototypeNode), std::move(functionBodyNode));
	
	auto statementEvent = FunctionTokenEvent {*functionDeclarationNode, FunctionTokenEventType::DECLARATION_STATEMENT, functionName.name() };
	m_parser.Observable<FunctionTokenEvent>::notifyObservers(statementEvent);

	return functionDeclarationNode;
}

ska::ASTNodePtr ska::MatcherFunction::matchCall(ASTNodePtr identifierFunctionName) {
	m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_BEGIN>());

	auto functionCallNodeContent = std::vector<ASTNodePtr>{};

	functionCallNodeContent.push_back(std::move(identifierFunctionName));
	
	const auto endParametersToken = m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>();
	while (!m_input.expect(endParametersToken)) {

		auto expressionOpt = m_parser.expr();
		if (expressionOpt != nullptr) {
			SLOG(ska::LogLevel::Debug) << "Expression not null";
			functionCallNodeContent.push_back(std::move(expressionOpt));
		} else {
			SLOG(ska::LogLevel::Debug) << "Expression null";
		}

        const auto commaToken = m_reservedKeywordsPool.pattern<TokenGrammar::ARGUMENT_DELIMITER>();
		if(m_input.expect(commaToken)) {
			m_input.match(commaToken);
		}
	}
	m_input.match(endParametersToken);

	auto functionCallNode = ASTNode::MakeNode<Operator::FUNCTION_CALL>(std::move(functionCallNodeContent));
	auto event = FunctionTokenEvent { *functionCallNode, FunctionTokenEventType::CALL };
	m_parser.Observable<FunctionTokenEvent>::notifyObservers(event);
	return functionCallNode;
}

ska::ASTNodePtr ska::MatcherFunction::matchDeclarationParameter() {
	const auto isRightParenthesis = m_input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>());
	if(isRightParenthesis) {
		return nullptr;
	}
	const auto& id = m_input.match(TokenType::IDENTIFIER);
	SLOG(ska::LogLevel::Debug) << id.name();
	
    m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::TYPE_DELIMITER>());
    const auto typeToken = 
		m_input.expect(TokenType::RESERVED) ?
		m_input.match(TokenType::RESERVED) : m_input.match(TokenType::IDENTIFIER);
	const auto typeStr = typeToken.name();

	SLOG(ska::LogLevel::Debug) << "type is : " << typeStr;
	
	//handle arrays
	auto typeNode = ASTNodePtr{};;
	if (m_input.expect(TokenType::ARRAY)) {
		auto arrayStartToken = m_input.match(TokenType::ARRAY);
		auto arrayEndToken = m_input.match(TokenType::ARRAY);
		if (std::get<std::string>(arrayStartToken.content()) != "[" ||
			std::get<std::string>(arrayEndToken.content()) != "]") {
			throw std::runtime_error("syntax error : only brackets [] are supported in a parameter declaration type");
		}

		typeNode = ASTNode::MakeLogicalNode(typeToken, ASTNode::MakeLogicalNode(Token{"", TokenType::ARRAY }));
	} else {
		typeNode = ASTNode::MakeLogicalNode(typeToken);
	}

	auto node = ASTNode::MakeNode<Operator::PARAMETER_DECLARATION>(id, std::move(typeNode));
	auto event = VarTokenEvent::MakeParameter(*node, (*node)[0]);
	m_parser.Observable<VarTokenEvent>::notifyObservers(event);
	return node;
}

std::vector<ska::ASTNodePtr> ska::MatcherFunction::fillDeclarationParameters() {
	m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_BEGIN>());

	auto parameters = std::vector<ASTNodePtr>{};
	auto isRightParenthesis = m_input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>());
	auto isComma = true;
	while (!isRightParenthesis && isComma) {
		if (!m_input.expect(TokenType::SYMBOL)) {
			SLOG(ska::LogLevel::Debug) << "parameter detected, reading identifier : ";
			auto parameterNode = matchDeclarationParameter();
			parameters.push_back(std::move(parameterNode));
			isComma = m_input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::ARGUMENT_DELIMITER>());
			if (isComma) {
				m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::ARGUMENT_DELIMITER>());
			}
		}
		isRightParenthesis = m_input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>());
	}
	m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>());
	
	return parameters;
}

ska::ASTNodePtr ska::MatcherFunction::matchDeclarationReturnType() {
	if (m_input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::TYPE_DELIMITER>())) {
		m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::TYPE_DELIMITER>());
		const auto type = m_input.match(TokenType::RESERVED);
		SLOG(ska::LogLevel::Debug) << "function type detected : " << type;
		//if(type.asString() != "var") {
		return ASTNode::MakeLogicalNode(type);
		//}
	} 

	SLOG(ska::LogLevel::Debug) << "void function detected";
	return ASTNode::MakeLogicalNode(ska::Token{ "", ska::TokenType::IDENTIFIER });
}

ska::ASTNodePtr ska::MatcherFunction::matchDeclarationBody() {
	m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_BEGIN>());

	auto statements = std::vector<ASTNodePtr>{};
	while (!m_input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_END>())) {
		auto optionalStatement = m_parser.statement();
		if (!optionalStatement->logicalEmpty()) {
			statements.push_back(std::move(optionalStatement));
		} else {
			break;
		}
	}
	m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_END>());

	auto blockNode = ASTNode::MakeNode<Operator::BLOCK>(std::move(statements));
	return blockNode;
}

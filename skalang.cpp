#include <iostream>
#include <sstream>
#include <iomanip>
#include <deque>
#include <cassert>
#include <stack>
#include <functional>

#include "Tokenizer.h"
#include "Scope.h"
#include "AST.h"
#include "ReservedKeywordsPool.h"
#include "ShuntingYardExpressionParser.h"

constexpr const char* TokenTypeSTR[] = {
	"RESERVED",
	"IDENTIFIER",
	"DIGIT",
	"SPACE",
	"STRING",
	"RANGE",
	"SYMBOL",
	"EMPTY",
	"UNUSED_LAST_Length"
};


namespace ska {
	
	class Parser {
	public:
		Parser(std::vector<Token> input) :
			m_input(std::move(input)),
			m_shuntingYardParser(m_input) {
		}

		std::pair<ASTNode, Scope> parse() {
			auto scope = ska::Scope { nullptr };
			auto ast = ska::ASTNode{};
			m_currentAst = &ast;
			m_currentScope = &scope;

			if (!m_input.empty()) {
				m_lookAhead = &m_input[0];
				expr();
			}
			m_currentAst = nullptr;
			m_currentScope = nullptr;
			return std::make_pair(std::move(ast), std::move(scope));
		}

	private:
		void statement() {
			if (m_lookAhead == nullptr) {
				return;
			}

			std::string* content = nullptr;
			switch (m_lookAhead->type()) {
			case TokenType::RESERVED:
				matchReservedKeyword(std::get<std::size_t>(m_lookAhead->content()));
				break;

			case TokenType::RANGE:
				matchRange(std::get<std::string>(m_lookAhead->content()));
				break;
			
			default:
				expr();
				break;
			}
		}

		void matchRange(const std::string& content) {
			if (content[0] == '{' ) {
				
				branch();
				match(Token{ "{", TokenType::RANGE });
				m_currentScope = &m_currentScope->add();
				do {
					optstatement();
				} while (m_lookAhead != nullptr && (*m_lookAhead) != Token{ "}", TokenType::RANGE });
				match(Token{ "}", TokenType::RANGE });
				unbranch();

				m_currentScope = &m_currentScope->parent();
			} else {
				optexpr(Token{ ";", TokenType::SYMBOL });
			}
		}

		void matchReservedKeyword(const std::size_t keywordIndex) {
			switch (keywordIndex) {

			case ReservedKeywords::FOR:
				match(Token{ ReservedKeywords::FOR, TokenType::RESERVED });
				branch();
				match(Token{ "(", TokenType::RANGE });			
				optstatement();
				unbranch();
				
				branch();
				optexpr(Token{ ";", TokenType::SYMBOL });
				match(Token{ ";", TokenType::SYMBOL });
				unbranch();
				
				branch();
				optexpr(Token{ ")", TokenType::SYMBOL });
				match(Token{ ")", TokenType::RANGE });
				unbranch();

				statement();
				break;
				
			case ReservedKeywords::ELSE:
				match(Token{ ReservedKeywords::ELSE, TokenType::RESERVED });
				statement();
				break;
			case ReservedKeywords::IF:
				match(Token{ ReservedKeywords::IF, TokenType::RESERVED });
				break;

			case ReservedKeywords::VAR:
				{
					match(Token{ ReservedKeywords::VAR, TokenType::RESERVED });
					const auto& identifier = match(TokenType::IDENTIFIER);
					match(Token{ "=", TokenType::SYMBOL });
					const auto& value = expr();
					match(Token{ ";", TokenType::SYMBOL });
					m_currentScope->registerIdentifier(std::get<std::string>(identifier.content()), value->token);
				}
				break;

			case ReservedKeywords::FUNCTION: {
					match(Token{ ReservedKeywords::FUNCTION, TokenType::RESERVED });
					match(Token{ "(", TokenType::RANGE });

					auto isRightParenthesis = m_lookAhead->type() == TokenType::SYMBOL && std::get<std::string>(m_lookAhead->content()) == ")";
					while (!isRightParenthesis) {
						if (m_lookAhead->type() == TokenType::SYMBOL) {
							expr();
							match(Token{ ",", TokenType::SYMBOL });
						}
						isRightParenthesis = m_lookAhead->type() == TokenType::SYMBOL && std::get<std::string>(m_lookAhead->content()) == ")";
					}

					match(Token{ ")", TokenType::RANGE });
				}
				break;

			default:
				error();
			}

		}

		std::unique_ptr<ska::ASTNode> expr() {
			return m_shuntingYardParser.parse(m_lookAheadIndex);
		}

		static void error() {
			throw std::runtime_error("syntax error");
		}

		const Token& match(const TokenType type) {
			if (m_lookAhead != nullptr && m_lookAhead->type() == type) {
				const auto& result = *m_lookAhead;
				//pushToken();
				nextToken();
				return result;
			}
			error();
		}

		Token match(Token t) {
			if (m_lookAhead != nullptr && *m_lookAhead == t) {
				return match(t.type());
			} else {
				error();
				return t;
			}
		}

		void pushToken() {
			assert(m_lookAhead != nullptr);
			if (m_lookAhead->type() != TokenType::RANGE) {
				//m_currentAst->addChild(*m_lookAhead);
			}
		}

		void unbranch() {
			//m_currentAst = &m_currentAst->parent;			
		}

		void branch() {
			assert(m_lookAhead != nullptr);
			if (m_currentAst != nullptr) {
				//auto& newToken = m_currentAst->addOtherChild(Token{});
				//m_currentAst = &newToken;
			}
		}

		void nextToken() {
			m_lookAhead = (m_lookAheadIndex + 1) < m_input.size() ? &m_input[++m_lookAheadIndex] : nullptr;
		}

		std::unique_ptr<ska::ASTNode> optexpr(const Token& mustNotBe = Token{}) {
			if (m_lookAhead != nullptr && (*m_lookAhead) != mustNotBe) {
				return expr();
			}
			return nullptr;
		}

		void optstatement(const Token& mustNotBe = Token{}) {
			if (m_lookAhead != nullptr && (*m_lookAhead) != mustNotBe) {
				statement();
			}
			else {
				match(Token{ ";", TokenType::SYMBOL });
			}
		}

		std::vector<Token> m_input;
		ShuntingYardExpressionParser m_shuntingYardParser;
		std::size_t m_lookAheadIndex = 0;
		Token* m_lookAhead = nullptr;
		
		ASTNode* m_currentAst;
		Scope* m_currentScope;
	};
}

void PrintTokenTreeRPN(const std::unique_ptr<ska::ASTNode>& node) {	
	
	
	if(node->left != nullptr) {
		PrintTokenTreeRPN(node->left);
	}
	
	if(node->right != nullptr) {
		PrintTokenTreeRPN(node->right);
	}
	
	std::cout << node->token.asString() << " ";
	
}

int main() {
	for (;;) {
		auto stop = false;
		std::stringstream ss;

		while (!stop) {
			auto line = std::string{};
			std::getline(std::cin, line);
			stop = line.empty();
			ss << line;
		}

		const auto reservedKeywords = ska::ReservedKeywordsPool{};

		auto tokenizer = ska::Tokenizer { reservedKeywords, ss.str() };
		auto tokens = tokenizer.tokenize();
		for (const auto& token : tokens) {
			std::cout << "[" << std::setw(10) << TokenTypeSTR[static_cast<std::size_t>(token.type())] << "]\t" << (token.type() == ska::TokenType::RESERVED ? "" : std::get<std::string>(token.content())) << std::endl;
		}

		auto parser = ska::Parser { std::move(tokens) };
		auto tokenTree = parser.parse();
		
		//PrintTokenTreeRPN(tokenTree);
		std::cout << std::endl;
	}
    return 0;
}


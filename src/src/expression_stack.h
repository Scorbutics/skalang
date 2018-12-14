#pragma once
#include <unordered_map>
#include <functional>
#include "iterable_stack.h"
#include "Token.h"
#include "AST.h"

namespace ska {
	
	template<class Operator, class Operand>
	class expression_stack {
		using PopPredicate = std::function<int(const Token&)>;
		static std::unordered_map<char, int> BuildPriorityMap();
		static std::unordered_map<char, int> PRIORITY_MAP;		

	public:
		void push(Operator&& op) {
			operators.push(std::forward<Operator>(op));
		}
		
		void push(Operand&& op) {
			operands.push(std::forward<Operand>(op));
		}
		
		[[nodiscard]]
		bool emptyOperator() const {
			return operators.empty();
		}
		
		[[nodiscard]]
		bool emptyOperands() const {
			return operands.empty();
		}
			
		[[nodiscard]]
		Operand& topOperand() {
			return operands.top();
		}

		void replaceTopOperand(Operand&& op) {
			operands.pop();
			operands.push(std::forward<Operand>(op));
		}
		
		Operand popOperandIfNoOperator(bool isMathOperator) {
			if(operands.empty()) {
				return nullptr;
			}

			if(isMathOperator) {
				throw std::runtime_error("syntax error : invalid operator placement");
			}

			return popOperand();
		}
		
		ska::ASTNodePtr popUntil(PopPredicate predicate) {
			auto currentNode = ASTNodePtr{};
			auto nextNode = ASTNodePtr{};

			if (emptyOperator() && !emptyOperands()) {
				return popOperand();
			}

			while (true) {
				if (emptyOperator() || emptyOperands()) {
					break;
				}

				const auto op = topOperator();
				const auto analyzeToken = predicate(op);
				//Flow control loop predicate by token
				if (analyzeToken < 0) {
					break;
				} else if (analyzeToken > 0) {
					popOperator();
					continue;
				}
				popOperator();

				auto rightOperand = popOperand();

				currentNode = ASTNodePtr{};
				if (nextNode != nullptr) {
					currentNode = ASTNode::MakeLogicalNode(op, std::move(nextNode), std::move(rightOperand));
				} else if (!emptyOperands()) {
					currentNode = ASTNode::MakeLogicalNode(op, popOperand(), std::move(rightOperand));
				}
				else {
					//TODO handle unary operator ?
					//assert(false && "Unsupported for now");
					//SLOG(ska::LogLevel::Debug) << "\t\tOperator " << op.asString();
					currentNode = ASTNode::MakeLogicalNode(op, /*ASTNode::MakeLogicalNode(Token { "0", TokenType::DIGIT }),*/ std::move(rightOperand));
				}

				nextNode = std::move(currentNode);
				//SLOG(ska::LogLevel::Debug) << "\t\tPoped " << op;//<< " with " <<  nextNode->left->token.asString() << " and " << nextNode->right->token.asString();
			}

			return (currentNode == nullptr  && nextNode == nullptr) ? nullptr : std::move(nextNode);
		}

		bool checkLessPriorityToken(const char tokenChar) const {
			const auto& topOperatorContent = (emptyOperator() || topOperator().name().empty()) ? '\0' : topOperator().name()[0];
			if (PRIORITY_MAP.find(tokenChar) == PRIORITY_MAP.end()) {
				auto ss = std::stringstream{};
				ss << "syntax error : bad operator : " << tokenChar;
				throw std::runtime_error(ss.str());
			}

			return PRIORITY_MAP.find(topOperatorContent) != PRIORITY_MAP.end() &&
				PRIORITY_MAP.at(tokenChar) < PRIORITY_MAP.at(topOperatorContent);
		}

		void triggerRangePoping(Token tok) {
			push(std::move(tok));
			auto rangeOperandResult = popUntil([](const Token& t) {
				const auto& strValue = t.name();
				if (t.type() == TokenType::RANGE) {
					return (strValue.empty() || strValue[0] == '(') ? -1 : 1;
				}
				return 0;
			});
			if (rangeOperandResult != nullptr) {
				push(std::move(rangeOperandResult));
			}
			if (!emptyOperator()) {
				popOperator();
			}
		}

	private:
		[[nodiscard]]
		Operator& topOperator() {
			return operators.top();
		}

		[[nodiscard]]
		const Operator& topOperator() const {
			return operators.top();
		}

		Operator popOperator() {
			auto result = std::move(operators.top());
			operators.pop();
			return result;
		}

		Operand popOperand() {
			auto result = std::move(operands.top());
			operands.pop();
			return result;
		}

		iterable_stack<Operator> operators {};
		iterable_stack<Operand> operands{};
	};

	template<class Operator, class Operand>
	std::unordered_map<char, int> ska::expression_stack<Operator, Operand>::BuildPriorityMap() {
		auto result = std::unordered_map<char, int>{};

		result.emplace('=', 50);
		result.emplace('>', 50);
		result.emplace('<', 50);
		result.emplace('+', 100);
		result.emplace('-', 100);
		result.emplace('*', 200);
		result.emplace('/', 200);

		return result;
	}
	template<class Operator, class Operand>
	std::unordered_map<char, int> ska::expression_stack<Operator, Operand>::PRIORITY_MAP = BuildPriorityMap();
}

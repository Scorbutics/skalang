#pragma once
#include <unordered_map>
#include <functional>
#include "iterable_stack.h"

namespace ska {
	namespace Group {
		enum class FlowControl {
			STOP,
			IGNORE_AND_CONTINUE,
			GROUP
		};

		template <class Operator>
		using FlowPredicate = std::function<FlowControl(const Operator&)>;

		template <class Operator>
		static FlowPredicate<Operator> All = [](const auto&) {return FlowControl::GROUP; };

		template <class Operator>
		static FlowPredicate<Operator> FirstOnly(bool& poped) {
			return [&poped](const auto& t) {
				if (poped) {
					return FlowControl::STOP;
				}
				poped = true;
				return FlowControl::GROUP;
			};
		};
	}

	template<class Operator, class Operand>
	class expression_stack {
		static std::unordered_map<char, int> BuildPriorityMap();
		static std::unordered_map<char, int> PRIORITY_MAP;		
	public:
		

		void push(Operator&& op) {
			operators.push(std::forward<Operator>(op));
		}
		
		void push(Operand&& op) {
			operands.push(std::forward<Operand>(op));
		}

		void replaceTopOperand(Operand&& op) {
			operands.pop();
			operands.push(std::forward<Operand>(op));
		}
		
		Operand popOperandIfNoOperator(bool isMathOperator) {
			if(operands.empty() || isMathOperator) {
				return nullptr;
			}

			return popOperand();
		}
		
		template <class NodeMaker>
		Operand groupAndPop(Group::FlowPredicate<Operator> predicate) {
			auto groupNode = Operand{};

            //No operator but only an operand : grouping is not necessary
			if (emptyOperator() && !emptyOperands()) {
				return popOperand();
			}

			while (true) {
                //Nothing to group
				if (emptyOperator() || emptyOperands()) {
					break;
				}
                
                const auto currentOperator = topOperator();
                const auto analyzeToken = predicate(currentOperator);
                if (analyzeToken == Group::FlowControl::STOP) {
                    break;
                } else if (analyzeToken != Group::FlowControl::IGNORE_AND_CONTINUE) {
                    groupNode = group <NodeMaker>(currentOperator, std::move(groupNode));
                }
                popOperator();
			}

			return groupNode;
		}

        template <class NodeMaker>
        Operand group(Operator currentOperator, Operand groupNode) {
            auto currentOperand = popOperand();

            auto currentNode = Operand{};
            if (groupNode != nullptr) {
                //Group already exists
                return NodeMaker::MakeLogicalNode(currentOperator, std::move(groupNode), std::move(currentOperand));
            } else if (!emptyOperands()) {
                //No group created, we make the first one
                return NodeMaker::MakeLogicalNode(currentOperator, popOperand(), std::move(currentOperand));
            }

            //TODO handle unary operator ?
            return NodeMaker::MakeLogicalNode(currentOperator, /*ASTNode::MakeLogicalNode(Token { "0", TokenType::DIGIT }),*/ std::move(currentOperand));
        }

		bool checkLessPriorityOperator(const char tokenOperatorChar) const {
			const auto& topOperatorContent = (emptyOperator() || topOperator().name().empty()) ? '\0' : topOperator().name()[0];
			if (PRIORITY_MAP.find(tokenOperatorChar) == PRIORITY_MAP.end()) {
				auto ss = std::stringstream{};
				ss << "syntax error : bad operator : " << tokenOperatorChar;
				throw std::runtime_error(ss.str());
			}

			return PRIORITY_MAP.find(topOperatorContent) != PRIORITY_MAP.end() &&
				PRIORITY_MAP.at(tokenOperatorChar) < PRIORITY_MAP.at(topOperatorContent);
		}

	private:
		[[nodiscard]]
		bool emptyOperator() const {
			return operators.empty();
		}

		[[nodiscard]]
		bool emptyOperands() const {
			return operands.empty();
		}

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

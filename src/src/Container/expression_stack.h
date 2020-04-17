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
		static std::unordered_map<std::string, int> BuildPriorityMap();
		static std::unordered_map<std::string, int> PRIORITY_MAP;
	public:
		void push(Operator&& op) {
			operators.push(std::forward<Operator>(op));
		}
		
		void push(Operand&& op) {
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

        	//Verify that maybe grouping is not necessary
			if (emptyOperator() && !emptyOperands()) {
				return popOperand();
			}

			while (true) {
            	//Nothing more to group, we stop
				if (emptyOperator()) {
					break;
				}
            	
            	const auto currentOperator = topOperator();
            	const auto analyzeToken = predicate(currentOperator);
            	if (analyzeToken == Group::FlowControl::STOP) {
                	break;
            	} else if (analyzeToken == Group::FlowControl::IGNORE_AND_CONTINUE) {
                	popOperator();
            	} else {
                	groupNode = makeGroup<NodeMaker>(std::move(groupNode));
				}
        	}

			return groupNode;
		}

		bool checkLessPriorityOperator(const std::string& tokenOperator) const {
			const std::string topOperatorContent = (emptyOperator() || topOperator().name().empty()) ? "" : topOperator().name();
			if (PRIORITY_MAP.find(tokenOperator) == PRIORITY_MAP.end()) {
				auto ss = std::stringstream{};
				ss << "syntax error : bad operator \"" << tokenOperator << "\"";
				throw std::runtime_error(ss.str());
			}

			return PRIORITY_MAP.find(topOperatorContent) != PRIORITY_MAP.end() &&
				PRIORITY_MAP.at(tokenOperator) < PRIORITY_MAP.at(topOperatorContent);
		}

	private:
    	template <class NodeMaker>
    	Operand addGroup(Operand groupNode) {
        	return NodeMaker::MakeLogicalNode(popOperator(), popOperand(), std::move(groupNode));
    	}

    	template <class NodeMaker>
    	Operand createGroup(bool binary) {
        	if (!binary) {
            	return NodeMaker::MakeLogicalNode(popOperator(), popOperand());
        	}

        	auto rightOperand = popOperand();
        	auto leftOperand = popOperand();
        	return NodeMaker::MakeLogicalNode(popOperator(), std::move(leftOperand), std::move(rightOperand));
    	}

    	template <class NodeMaker>
    	Operand makeGroup(Operand groupNode) {
        	if (groupNode != nullptr) {
            	return addGroup <NodeMaker>(std::move(groupNode));
        	}
        	
        	return createGroup <NodeMaker>(operands.size() >= 2);
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
		Operator& topOperator() {
			assert(!operators.empty());
			return operators.top();
		}

		[[nodiscard]]
		const Operator& topOperator() const {
			assert(!operators.empty());
			return operators.top();
		}

		Operator popOperator() {
			assert(!operators.empty());
			auto result = std::move(operators.top());
			operators.pop();
			return result;
		}

		Operand popOperand() {
			assert(!operands.empty());
			auto result = std::move(operands.top());
			operands.pop();
			return result;
		}

		iterable_stack<Operator> operators {};
		iterable_stack<Operand> operands{};
	};

	template<class Operator, class Operand>
	std::unordered_map<std::string, int> ska::expression_stack<Operator, Operand>::BuildPriorityMap() {
		auto result = std::unordered_map<std::string, int>{};

		result.emplace("==", 40);
		result.emplace("!=", 40);
		result.emplace("<=", 40);
		result.emplace(">=", 40);
		result.emplace(">", 40);
		result.emplace("<", 40);
		result.emplace("=", 50);
		result.emplace("-=", 50);
		result.emplace("+=", 50);
		result.emplace("*=", 50);
		result.emplace("++", 100);
		result.emplace("+", 100);
		result.emplace("--", 100);
		result.emplace("-", 100);
		result.emplace("*", 200);
		result.emplace("/", 200);
		result.emplace(":", 300);

		return result;
	}
	template<class Operator, class Operand>
	std::unordered_map<std::string, int> ska::expression_stack<Operator, Operand>::PRIORITY_MAP = BuildPriorityMap();

}

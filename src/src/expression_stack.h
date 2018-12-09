#pragma once
#include "iterable_stack.h"

namespace ska {
	
	template<class Operator, class Operand>
	class expression_stack {
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
		Operator& topOperator() {
			return operators.top();
		}
		
		Operator popOperator() {
			auto result = std::move(operators.top());
			operators.pop();
			return result;
		}
		
		[[nodiscard]]
		Operand& topOperand() {
			return operands.top();
		}

		Operand popOperand() {
			auto result = std::move(operands.top());
			operands.pop();
			return result;
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
		
	private:
		iterable_stack<Operator> operators {};
		iterable_stack<Operand> operands{};
	};
}
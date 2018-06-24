#include <iostream>

#include <memory>
#include <unordered_map>
#include <functional>
#include <string>
#include <cassert>

#include <sstream>
#include <algorithm>
#include <cctype>

namespace ska {
	std::vector<std::string>& split(const std::string &s, const char delim, std::vector<std::string> &elems) {
		std::stringstream ss(s);
		std::string item;
		while (getline(ss, item, delim)) {
			elems.push_back(item);
		}
		return elems;
	}

	std::vector<std::string> split(const std::string &s, const char delim) {
		std::vector<std::string> elems;
		split(s, delim, elems);
		return elems;
	}

	// trim from start
	std::string ltrim(const std::string &s) {
		auto tmp = std::string(s.c_str());
		auto firstCharIt = find_if(tmp.begin(), tmp.end(), [](const auto& c) {return !std::isspace(c); });
		tmp.erase(tmp.begin(), firstCharIt);
		return tmp;
	}

	// trim from end
	std::string rtrim(const std::string &s) {
		std::string tmp(s.c_str());
		auto lastCharIt = std::find_if(tmp.rbegin(), tmp.rend(), [](const auto& c) {return !std::isspace(c); }).base();
		tmp.erase(lastCharIt, tmp.end());
		return tmp;
	}

	// trim from both ends
	std::string trim(const std::string &s) {
		return ltrim(rtrim(s));
	}

}

namespace ska {
	//TODO gestion des parenthèses
	static const constexpr char* Operators[] = { "&&", "||", "!=", "==", "<=", "<", ">", ">=" };

	enum class OperatorType {
		Numeric,
		Logical,
		Root
	};

	struct ASTNode {
		OperatorType type = OperatorType::Root;
		std::string value;
		std::string operator_;
		std::unique_ptr<ASTNode> childLeft;
		std::unique_ptr<ASTNode> childRight;
		int rangeMin = 0;
		int rangeMax = -1;
	};

	struct LogicalOperator {
		using ComparisonPredicate = std::function<bool(int, int)>;

		ComparisonPredicate check;
		int priority;
	};

	std::unordered_map<std::string, LogicalOperator> BuildOperators() {
		auto result = std::unordered_map<std::string, LogicalOperator> {};
		result.emplace(Operators[0], LogicalOperator{ [](auto op1, auto op2) {
			return op1 && op2;
		}, 100 });

		result.emplace(Operators[1], LogicalOperator{ [](auto op1, auto op2) {
			return op1 || op2;
		}, 50 });

		result.emplace(Operators[2], LogicalOperator{ [](auto op1, auto op2) {
			return op1 != op2;
		}, 0 });

		result.emplace(Operators[3], LogicalOperator{ [](auto op1, auto op2) {
			return op1 == op2;
		}, 0 });

		result.emplace(Operators[4], LogicalOperator{ [](auto op1, auto op2) {
			return op1 <= op2;
		}, 1000 });

		result.emplace(Operators[5], LogicalOperator{ [](auto op1, auto op2) {
			return op1 < op2;
		}, 1000 });

		result.emplace(Operators[6], LogicalOperator{ [](auto op1, auto op2) {
			return op1 > op2;
		}, 1000 });

		result.emplace(Operators[7], LogicalOperator{ [](auto op1, auto op2) {
			return op1 >= op2;
		}, 1000 });
		
		return result;
	}

	auto m_operators = BuildOperators();

	void BuildAST(ASTNode& node, const std::vector<std::string>& expressionParts, const std::unordered_map<std::string, LogicalOperator>& operators) {
		auto maxPriorityLogicalOperator = std::string{};
		auto maxPriorityLogicalOperatorValue = -1;
		auto maxPriorityLogicalOperatorIndex = -1;

		const auto& maxRange = node.rangeMax == -1 ? expressionParts.size() - 1 : node.rangeMax;
		for (auto index = node.rangeMin; index <= maxRange; index++) {
			const auto& part = expressionParts[index];
			if (operators.find(part) != operators.end()) {
				if (operators.at(part).priority > maxPriorityLogicalOperatorValue) {
					maxPriorityLogicalOperator = part;
					maxPriorityLogicalOperatorValue = operators.at(part).priority;
					maxPriorityLogicalOperatorIndex = index;
				}
			}
		}

		if (maxPriorityLogicalOperatorIndex >= 0 && maxPriorityLogicalOperatorIndex < expressionParts.size()) {
			node.type = OperatorType::Logical;
			node.operator_ = maxPriorityLogicalOperator;
			node.childLeft = std::make_unique<ASTNode>();
			node.childLeft->rangeMin = node.rangeMin;
			node.childLeft->rangeMax = maxPriorityLogicalOperatorIndex - 1;

			node.childRight = std::make_unique<ASTNode>();
			node.childRight->rangeMin = maxPriorityLogicalOperatorIndex + 1;
			node.childRight->rangeMax = node.rangeMax;
			BuildAST(*node.childLeft, expressionParts, operators);
			BuildAST(*node.childRight, expressionParts, operators);
		} else {
			node.type = OperatorType::Numeric;
			node.value = expressionParts[node.rangeMin];
		}
	}

	ASTNode CreateAST(const std::vector<std::string>& expressionParts, const std::unordered_map<std::string, LogicalOperator>& operators) {
		auto root = ASTNode{};
		BuildAST(root, expressionParts, operators);
		return root;
	}

	int InterpretAST(const ASTNode& ast, const std::unordered_map<std::string, LogicalOperator>& operators) {
		if (ast.type == OperatorType::Logical) {
			assert(ast.childLeft != nullptr);
			auto left = InterpretAST(*ast.childLeft, operators);

			assert(ast.childRight != nullptr);
			auto right = InterpretAST(*ast.childRight, operators);

			return operators.at(ast.operator_).check(left, right);
		}
		return std::stoi(ast.value);
	}
}

int main() {
	auto stop = false;
	while (!stop) {
		auto line = std::string {};
		std::getline(std::cin, line);
	
		const auto expressionParts = ska::split(line, ' ');
		auto ast = ska::CreateAST(expressionParts, ska::m_operators);
		auto result = ska::InterpretAST(ast, ska::m_operators);
		
		std::cout << result << std::endl;
	}
	
    return 0;
}


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
	static const constexpr char NumericalOperators[] = { '+', '-', '*', '/', '%' };
	static const constexpr char* LogicalOperators[] = { "&&", "||", "!=", "==", "<=", "<", ">", ">=" };
	static const constexpr char RangeOperators[] = { '(', ')', '"' };
	static const constexpr auto AffectationOperator = '=';

	enum class OperatorType {
		Numeric,
		Logical,
		Arithmetic,
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

	struct NumericalOperator {
		using NumericalFunction = std::function<float(float, float)>;

		NumericalFunction operate;
		int priority;
	};

	std::unordered_map<std::string, LogicalOperator> BuildOperators() {
		auto result = std::unordered_map<std::string, LogicalOperator> {};
		result.emplace(LogicalOperators[0], LogicalOperator{ [](auto op1, auto op2) {
			return op1 && op2;
		}, 100 });

		result.emplace(LogicalOperators[1], LogicalOperator{ [](auto op1, auto op2) {
			return op1 || op2;
		}, 50 });

		result.emplace(LogicalOperators[2], LogicalOperator{ [](auto op1, auto op2) {
			return op1 != op2;
		}, 0 });

		result.emplace(LogicalOperators[3], LogicalOperator{ [](auto op1, auto op2) {
			return op1 == op2;
		}, 0 });

		result.emplace(LogicalOperators[4], LogicalOperator{ [](auto op1, auto op2) {
			return op1 <= op2;
		}, 1000 });

		result.emplace(LogicalOperators[5], LogicalOperator{ [](auto op1, auto op2) {
			return op1 < op2;
		}, 1000 });

		result.emplace(LogicalOperators[6], LogicalOperator{ [](auto op1, auto op2) {
			return op1 > op2;
		}, 1000 });

		result.emplace(LogicalOperators[7], LogicalOperator{ [](auto op1, auto op2) {
			return op1 >= op2;
		}, 1000 });
		return result;
	}


	std::unordered_map<char, NumericalOperator> BuildNumericalOperators() {
		auto result = std::unordered_map<char, NumericalOperator>{};
		result.emplace(NumericalOperators[0], NumericalOperator{ [](auto op1, auto op2) {
			return op1 + op2;
		}, 0 });

		result.emplace(NumericalOperators[1], NumericalOperator{ [](auto op1, auto op2) {
			return op1 - op2;
		}, 0 });

		result.emplace(NumericalOperators[2], NumericalOperator{ [](auto op1, auto op2) {
			return op1 * op2;
		}, 100 });

		result.emplace(NumericalOperators[3], NumericalOperator{ [](auto op1, auto op2) {
			return op1 / op2;
		}, 100 });

		result.emplace(NumericalOperators[4], NumericalOperator{ [](auto op1, auto op2) {
			return static_cast<int>(op1) % static_cast<int>(op2);
		}, 10 });

		return result;
	}
	auto logicalOperators = BuildOperators();
	auto numericalOperators = BuildNumericalOperators();

	void BuildAST(ASTNode& node, const std::vector<std::string>& expressionParts) {
		auto maxPriorityLogicalOperator = std::string{};
		auto maxPriorityLogicalOperatorValue = -1;
		auto maxPriorityLogicalOperatorIndex = -1;

		const auto& maxRange = node.rangeMax == -1 ? expressionParts.size() - 1 : node.rangeMax;
		for (auto index = node.rangeMin; index <= maxRange; index++) {
			const auto& part = expressionParts[index];
			if (logicalOperators.find(part) != logicalOperators.end()) {
				if (logicalOperators.at(part).priority > maxPriorityLogicalOperatorValue) {
					maxPriorityLogicalOperator = part;
					maxPriorityLogicalOperatorValue = logicalOperators.at(part).priority;
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
			BuildAST(*node.childLeft, expressionParts);
			BuildAST(*node.childRight, expressionParts);
		} else {
			node.type = OperatorType::Numeric;
			node.value = expressionParts[node.rangeMin];
		}
	}

	struct Range {
	    bool real = true;
		int min = 0;
		int size = 0;
		std::vector<std::unique_ptr<Range>> next;
		Range* parent = this;
	};

	template <char RangeOperatorLeft, char RangeOperatorRight>
	struct RangeDetector {
	    static std::unique_ptr<Range> detect(const std::string& line, int startIndex = 0) {
            if(startIndex < line.size()) {
                const auto lineStart = line.cbegin() + startIndex;
                auto range = std::make_unique<Range>();
                auto index = startIndex - 1;
                auto currentRangeOperatorCounter = 0;

                for (auto it = lineStart; it != line.cend(); it++) {
                    index++;
                    switch (*it) {
                    case RangeOperatorLeft:
                        if (currentRangeOperatorCounter == 0) {
                            range->min = index + 1;
                        } else {
                            range->next.push_back(detect(line, index));
                            auto& lastAdded = range->next.back();
                            lastAdded->parent = range.get();
                        }
                        currentRangeOperatorCounter++;
                        break;

                    case RangeOperatorRight:
                        currentRangeOperatorCounter--;
                        if (currentRangeOperatorCounter == 0) {
                            auto& lastAdded = range->next.empty() ? (*range) : (*range->next.back().get());
                            range->size = index - 1 - startIndex;
                            return std::move(range);
                        }
                        break;

                    default:
                        continue;
                    }
                }
                return std::move(range);
            }
            return nullptr;
        }
	};


    template <char RangeOperatorLeft>
    struct RangeDetector<RangeOperatorLeft, RangeOperatorLeft> {
        static std::unique_ptr<Range> detect(const std::string& line, int startIndex = 0) {
            if(startIndex < line.size()) {
                const auto lineStart = line.cbegin() + startIndex;
                auto range = std::make_unique<Range>();
                auto index = startIndex - 1;
                auto currentRangeOperatorCounter = true;

                for (auto it = lineStart; it != line.cend(); it++) {
                    index++;
                    switch (*it) {
                    case RangeOperatorLeft:
                        if (currentRangeOperatorCounter) {
                            range->min = index + 1;
                        } else {
                            range->size = index - 1 - startIndex;
                            return std::move(range);
                        }

                        currentRangeOperatorCounter = !currentRangeOperatorCounter;
                        break;

                    default:
                        continue;
                    }
                }
                return nullptr;
            }
            return nullptr;
        }
    };

    template <class TreeNode>
    void ActOnTreeChildrenFirst(const TreeNode& rangeTree, const std::function<void(const TreeNode& r)>& func) {
        for (const auto& r : rangeTree.next) {
            if(r != nullptr){
                ActOnTreeChildrenFirst(*r, func);
            }
		}
		func(rangeTree);
    }

	ASTNode CreateAST(const std::string& line) {
		auto root = ASTNode{};

        std::cout << "String Litterals :" << std::endl;
		auto stringRanges = RangeDetector<RangeOperators[2], RangeOperators[2]>::detect(line);
        if(stringRanges != nullptr) {
            ActOnTreeChildrenFirst<Range>(*stringRanges, [&](const Range& rangeIt) {
               std::cout << line.substr(rangeIt.min, rangeIt.size) << std::endl;
                /*const auto expressionParts = ska::split(line.substr(rangeIt.min + 1, rangeIt.min + rangeIt.max - 2), ' ');
                for(const auto& e : expressionParts) {
                    std::cout << e << std::endl;
                }*/
            });
        }

		std::cout << "Parenthesis :" << std::endl;
		auto parenthesisRanges = RangeDetector<RangeOperators[0], RangeOperators[1]>::detect(line);
        if(parenthesisRanges != nullptr) {
            ActOnTreeChildrenFirst<Range>(*parenthesisRanges, [&](const Range& rangeIt) {
                std::cout << line.substr(rangeIt.min, rangeIt.size) << std::endl;
            });
        }
		return root;
	}

	int InterpretAST(const ASTNode& ast) {
		if (ast.type == OperatorType::Logical) {
			assert(ast.childLeft != nullptr);
			auto left = InterpretAST(*ast.childLeft);

			assert(ast.childRight != nullptr);
			auto right = InterpretAST(*ast.childRight);

			return logicalOperators.at(ast.operator_).check(left, right);
		}
		if (ast.type == OperatorType::Arithmetic) {
			assert(ast.childLeft != nullptr);
			auto left = InterpretAST(*ast.childLeft);

			assert(ast.childRight != nullptr);
			auto right = InterpretAST(*ast.childRight);

			return numericalOperators.at(ast.operator_[0]).operate(left, right);
		}
		return std::stoi(ast.value);
	}
}

int main() {
	auto stop = false;
	while (!stop) {
		auto line = std::string {};
		std::getline(std::cin, line);


		auto ast = ska::CreateAST(line);
		//auto result = ska::InterpretAST(ast);

		//std::cout << result << std::endl;
	}

    return 0;
}


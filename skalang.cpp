#include <iostream>

#include <memory>
#include <unordered_map>
#include <functional>
#include <string>
#include <cassert>
#include <bitset>
#include <iomanip>

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
		SubRange,
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

    template <class TreeNode>
    void ActOnTreeRootFirst(const TreeNode& rangeTree, const std::function<void(const TreeNode& r)>& func) {
        func(rangeTree);
        for (const auto& r : rangeTree.next) {
            if(r != nullptr){
                ActOnTreeRootFirst(*r, func);
            }
		}
    }

    std::vector<Range*> BuildRangeCollisionSegment(const Range& rangeIt) {
        auto allowed = std::vector<Range*>();
        allowed.resize(rangeIt.size + 1);

        for(auto i = rangeIt.min; i < rangeIt.size + rangeIt.min; i++) {
            for(const auto& child: rangeIt.next) {
                if(child != nullptr && i >= child->min && i < (child->min + child->size)) {
                    i = child->min + child->size;
                    allowed[child->min - rangeIt.min] = child.get();
                    break;
                }
            }
        }
        return allowed;
    }

	enum class TokenType {
		REQUIRED,
		IDENTIFIER,
		DIGIT,
		SPACE,
		STRING,
		RANGE,
		SYMBOL,
		EMPTY,
		UNUSED_LAST_Length
	};

	constexpr const char* TokenTypeSTR[]  = {
		"REQUIRED",
		"IDENTIFIER",
		"DIGIT",
		"SPACE",
		"STRING",
		"RANGE",
		"SYMBOL",
		"EMPTY",
		"UNUSED_LAST_Length"
	};

	struct Token {
		std::string content;
		TokenType type;
	};

	class Tokenizer {
	public:
		Tokenizer(std::string input) :
			m_input(std::move(input)) {
		}

		Token tokenize(std::size_t index = 0) {
			if (index == 0) {
				index = m_startIndex;
			}

			if (index >= m_input.size()) {
				return finalizeToken(index);
			}

			const auto charTokenType = calculateCharacterTokenType(m_input[index]);
			const auto isFirstCharacterOfToken = m_startIndex == index;
			if (
				(m_required && !m_requiredOrUntil[static_cast<std::size_t>(charTokenType)] 
				|| !m_required && m_requiredOrUntil[static_cast<std::size_t>(charTokenType)]) 
				&& !isFirstCharacterOfToken) {
				return finalizeToken(index);
			}
			
			if (isFirstCharacterOfToken) {
				m_currentToken = charTokenType;
			}


			switch (charTokenType) {
			case TokenType::DIGIT:
				if (isFirstCharacterOfToken) {
					m_required = true;
					m_requiredOrUntil[static_cast<std::size_t>(TokenType::DIGIT)] = true;
				}
				return tokenize(index + 1);

			case TokenType::SPACE:
				if (isFirstCharacterOfToken) {
					m_required = true;
					m_startIndex = index + 1;
				}
				return tokenize(index + 1);

			case TokenType::SYMBOL:
				if (isFirstCharacterOfToken) {
					m_required = true;
					m_requiredOrUntil[static_cast<std::size_t>(TokenType::SYMBOL)] = true;
				}
				return tokenize(index + 1);

			case TokenType::RANGE:
				if (isFirstCharacterOfToken) {
					m_required = true;
				}
				return tokenize(index + 1);

			case TokenType::STRING:
				if (isFirstCharacterOfToken) {
					m_required = false;
					m_requiredOrUntil[static_cast<std::size_t>(TokenType::STRING)] = true;
				}
				return tokenize(index + 1);

			default:
			case TokenType::IDENTIFIER:
				if (isFirstCharacterOfToken) {
					m_required = true;
					m_requiredOrUntil[static_cast<std::size_t>(TokenType::DIGIT)] = true;
					m_requiredOrUntil[static_cast<std::size_t>(TokenType::IDENTIFIER)] = true;
				}
				return tokenize(index + 1);
			}
		}

	private:
		Token finalizeToken(std::size_t index)  {
			auto token = Token{};

			token.type = m_currentToken;
			m_currentToken = TokenType::EMPTY;
			index += (!m_required ? 1 : 0);
			token.content = m_input.substr(m_startIndex, index - m_startIndex);
			m_startIndex = index;
			m_requiredOrUntil.reset();
			m_required = true;

			return token;
		}

		TokenType calculateCharacterTokenType(const char c) const {
			switch (c) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				return TokenType::DIGIT;
			
			case ' ':
			case '\t':
			case '\v':
			case '\f':
			case '\n':
			case '\r':
				return TokenType::SPACE;
			
			case '"':
				return TokenType::STRING;

			case '(':
			case ')':
			case '{':
			case '}':
				return TokenType::RANGE;

			default :
				return isWordCharacter(c) ? TokenType::IDENTIFIER : TokenType::SYMBOL;
			}
		}

		static bool isWordCharacter(const int c) {
			return std::isalnum(c) || c == '_';
		}

		std::size_t m_startIndex = 0;
		std::string m_input;
		TokenType m_currentToken = TokenType::EMPTY;
		bool m_required;
		std::bitset<static_cast<std::size_t>(TokenType::UNUSED_LAST_Length)> m_requiredOrUntil;
	};

	template <class TreeNode>
	void ActOnTokenRootFirst(const TreeNode& rangeTree, const std::function<void(const TreeNode& r)>& func) {
		func(rangeTree);
		for (const auto& r : rangeTree.subtoken) {
			ActOnTokenRootFirst(r, func);			
		}
	}

	template <class TreeNode>
	void ActOnTokenChildrenFirst(const TreeNode& rangeTree, const std::function<void(const TreeNode& r)>& func) {
		for (const auto& r : rangeTree.subtoken) {
			ActOnTokenChildrenFirst(r, func);
		}
		func(rangeTree);
	}

	/*ASTNode CreateAST(const std::string& line) {
		auto root = ASTNode{};

        std::cout << "String Litterals :" << std::endl;
		auto stringRanges = RangeDetector<RangeOperators[2], RangeOperators[2]>::detect(line);
        if(stringRanges != nullptr) {
            ActOnTreeRootFirst<Range>(*stringRanges, [&](const Range& rangeIt) {
               std::cout << line.substr(rangeIt.min, rangeIt.size) << std::endl;
                const auto expressionParts = ska::split(line.substr(rangeIt.min + 1, rangeIt.min + rangeIt.max - 2), ' ');
                for(const auto& e : expressionParts) {
                    std::cout << e << std::endl;
                }
            });
        }

		std::cout << "Parenthesis :" << std::endl;


		for (auto& root : rootToken) {
			ActOnTokenChildrenFirst<Token>(root, [&](const auto& r) {
				std::cout << r.letter;
			});
			std::cout << std::endl;
		}

		return root;
	}*/

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


		std::cout << std::endl;

		auto tokenizer = ska::Tokenizer{ line };
		
		ska::Token token;
		do {
			token = tokenizer.tokenize();
			std::cout << "[" << std::setw(10)<< ska::TokenTypeSTR[static_cast<std::size_t>(token.type)] << "]\t" << token.content << std::endl;
		} while (token.type != ska::TokenType::EMPTY);
	}

    return 0;
}


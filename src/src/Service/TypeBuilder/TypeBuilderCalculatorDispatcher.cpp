#include <iostream>

#include "Config/LoggerConfigLang.h"
#include "Service/SymbolTable.h"
#include "NodeValue/AST.h"

namespace ska {
	struct TypeBuilderBuildFromTokenTypeTag;

    Type TypeBuilderBuildFromTokenType(const SymbolTable& symbols, const ASTNode& node) {
        switch(node.tokenType()) {
            case TokenType::SYMBOL:
            case TokenType::SPACE:
            case TokenType::RANGE:
            case TokenType::DOT_SYMBOL:
				return Type{};

            case TokenType::STRING:
				return Type::MakeBuiltIn<ExpressionType::STRING>();

            case TokenType::DIGIT: {
                const auto isDecimal = node.name().find_first_of('.') != std::string::npos;
				return isDecimal ? Type::MakeBuiltIn<ExpressionType::FLOAT>() : Type::MakeBuiltIn<ExpressionType::INT>();
            }

            case TokenType::IDENTIFIER: {
                const auto symbol = symbols[node.name()];
				if (symbol != nullptr) {
					if (symbol->getType().hasSymbol()) {
						return symbol->getType();
					}
					const auto* symbol = symbols[node.name()];
					const auto* finalSymbol = symbol == nullptr ? nullptr : (*symbol)[node.name()];

					switch (symbol->getType().type()) {
					case ExpressionType::OBJECT:
						return Type::MakeCustom<ExpressionType::OBJECT>(finalSymbol);
					case ExpressionType::FUNCTION:
						return Type::MakeCustom<ExpressionType::FUNCTION>(finalSymbol);
					default:
						return symbol->getType();
					}
				}
				return Type{};
            }
			case TokenType::BOOLEAN:
				return Type::MakeBuiltIn<ExpressionType::BOOLEAN>();

            case TokenType::RESERVED: {
				auto type = Type{};
                //TODO map
                  if(node.name() == std::string(ExpressionTypeSTR[static_cast<std::size_t>(ExpressionType::INT)])) {
					  type = Type::MakeBuiltIn<ExpressionType::INT>();
                  } else if (node.name() == std::string(ExpressionTypeSTR[static_cast<std::size_t>(ExpressionType::FLOAT)])) {
					  type = Type::MakeBuiltIn<ExpressionType::FLOAT>();
                  } else if (node.name() == std::string(ExpressionTypeSTR[static_cast<std::size_t>(ExpressionType::STRING)])) {
					  type = Type::MakeBuiltIn<ExpressionType::STRING>();
                  } else if (node.name() == std::string(ExpressionTypeSTR[static_cast<std::size_t>(ExpressionType::BOOLEAN)])) {
					  type = Type::MakeBuiltIn<ExpressionType::FLOAT>();
                  } else if(node.name() == std::string(ExpressionTypeSTR[static_cast<std::size_t>(ExpressionType::OBJECT)])) {
					  type = Type::MakeCustom<ExpressionType::OBJECT>(nullptr);
                  } else if (node.name() == std::string(ExpressionTypeSTR[static_cast<std::size_t>(ExpressionType::FUNCTION)])) {					  
					  type = Type::MakeCustom<ExpressionType::FUNCTION>(nullptr);
				  } else if (node.name() == "true" || node.name() == "false") {
                      type = Type::MakeBuiltIn<ExpressionType::BOOLEAN>();
                  }
				  //handles arrays
				  if (node.size() == 1 && node[0].tokenType() == TokenType::ARRAY) {
					  type = Type::MakeBuiltIn<ExpressionType::ARRAY>().add(type);
				  }
				  return type;
            }
			case TokenType::ARRAY:
				return Type::MakeBuiltIn<ExpressionType::ARRAY>();

            default:
				assert(!"Unhandled type for this node");
                break;
        }

        SLOG_STATIC(ska::LogLevel::Error, TypeBuilderBuildFromTokenTypeTag) << "default type returned for node \"" << node.name() << "\" of type " << TokenTypeSTR[static_cast<std::size_t>(node.tokenType())];

		return Type{ };
            
    }
       
 
}

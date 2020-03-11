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
					if (symbol->linkedTypeHasSymbol()) {
						return symbol->type();
					}
					const auto* finalSymbol = (*symbol)[node.name()];
					switch (symbol->nativeType()) {
					case ExpressionType::OBJECT:
						return Type::MakeCustom<ExpressionType::OBJECT>(finalSymbol);
					case ExpressionType::FUNCTION:
						return Type::MakeCustom<ExpressionType::FUNCTION>(finalSymbol);
					default:
						return symbol->type();
					}
				}
				return Type{};
            }
			case TokenType::BOOLEAN:
				return Type::MakeBuiltIn<ExpressionType::BOOLEAN>();

            case TokenType::RESERVED: {
				auto type = Type{};
				if(ExpressionTypeMap.find(node.name()) != ExpressionTypeMap.end()) {
					switch(ExpressionTypeMap.at(node.name())) {
						case ExpressionType::INT:
						type = Type::MakeBuiltIn<ExpressionType::INT>();
						break;
						case ExpressionType::FLOAT:
						type = Type::MakeBuiltIn<ExpressionType::FLOAT>();
						break;
						case ExpressionType::STRING:
						type = Type::MakeBuiltIn<ExpressionType::STRING>();
						break;
						case ExpressionType::BOOLEAN:
						type = Type::MakeBuiltIn<ExpressionType::BOOLEAN>();
						break;
						case ExpressionType::OBJECT:
						type = Type::MakeCustom<ExpressionType::OBJECT>(nullptr);
						break;
                  		case ExpressionType::FUNCTION:
					  	type = Type::MakeCustom<ExpressionType::FUNCTION>(nullptr);
						break;
						default:
						if(node.name() == "true" || node.name() == "false") {
							type = Type::MakeBuiltIn<ExpressionType::BOOLEAN>();
						}
						break;
					}
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

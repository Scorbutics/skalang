#include <iostream>
#include "Config/LoggerConfigLang.h"
#include "Service/StatementParser.h"
#include "SemanticTypeChecker.h"
#include "SymbolTable.h"
#include "NodeValue/AST.h"
#include "NodeValue/OperatorTraits.h"
#include "Interpreter/Value/Script.h"
#include "NodeValue/Type.h"

#include "Operation/Type/OperationTypeFunctionDeclaration.h"
#include "Operation/Type/OperationTypeFunctionCall.h"
#include "Operation/Type/OperationTypeIfElse.h"
#include "Operation/Type/OperationTypeArrayDeclaration.h"
#include "Operation/Type/OperationTypeReturn.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::SemanticTypeChecker)

ska::SemanticTypeChecker::SemanticTypeChecker(StatementParser& parser, const TypeCrosser& typeCrosser) :
    subobserver_priority_queue<VarTokenEvent>(std::bind(&SemanticTypeChecker::matchVariable, this, std::placeholders::_1), parser, 9),
    subobserver_priority_queue<FunctionTokenEvent>(std::bind(&SemanticTypeChecker::matchFunction, this, std::placeholders::_1), parser, 9),
	subobserver_priority_queue<ArrayTokenEvent>(std::bind(&SemanticTypeChecker::matchArray, this, std::placeholders::_1), parser, 9),
    subobserver_priority_queue<ReturnTokenEvent>(std::bind(&SemanticTypeChecker::matchReturn, this, std::placeholders::_1), parser, 9),
	subobserver_priority_queue<IfElseTokenEvent>(std::bind(&SemanticTypeChecker::matchIfElse, this, std::placeholders::_1), parser, 9),
	m_typeCrosser(typeCrosser) {
}

bool ska::SemanticTypeChecker::statementHasReturnOnAllControlPath(const ASTNode& node) {	
	switch (node.op()) {
	case Operator::FUNCTION_DECLARATION:
	case Operator::FUNCTION_PROTOTYPE_DECLARATION:
	case Operator::IF:
		//false, cause even if there was a return inside children nodes, in this case it would not be enough to guaranty the systematic return
		return false;
	case Operator::IF_ELSE: {
		auto ifElseOperation = OperationType<Operator::IF_ELSE>{ node };
		return statementHasReturnOnAllControlPath(ifElseOperation.GetIfStatement()) && statementHasReturnOnAllControlPath(ifElseOperation.GetElseStatement());
	}
	case Operator::RETURN:
		return true;
	default:
		return childrenHasReturnOnAllControlPath(node);
	}
}

bool ska::SemanticTypeChecker::childrenHasReturnOnAllControlPath(const ASTNode& node) {
	auto result = false;
	for (const auto& statement : node) {
		result |= statementHasReturnOnAllControlPath(*statement);
		if (result) {
			break;
		}
	}

	return result;
}

bool ska::SemanticTypeChecker::matchReturn(const ReturnTokenEvent& token) {
    switch(token.type()) {
        case ReturnTokenEventType::START:
            break;

        case ReturnTokenEventType::BUILTIN:
		case ReturnTokenEventType::OBJECT: {
			const auto symbol = token.script().symbols().enclosingType();
			auto* finalSymbol = token.script().symbols()[symbol->getName()];
			if (symbol == nullptr || symbol->getName().empty() || finalSymbol == nullptr) {
				throw std::runtime_error("return must be place in a function block or a nested one");
			}

			const auto type = finalSymbol->getType();
			auto operationReturn = OperationType<Operator::RETURN>{token.rootNode()};
			const auto& returnedValue = operationReturn.GetValue();
			if (type.compound().empty() || !returnedValue.type().has_value()) {
				throw std::runtime_error("\"" + symbol->getName() + "\" is not a function");
			}

			const auto expectedReturnType = type.compound().back();
			if (((returnedValue.op() == Operator::USER_DEFINED_OBJECT) && (expectedReturnType != ExpressionType::OBJECT)) || 
				(returnedValue.op() != Operator::USER_DEFINED_OBJECT && expectedReturnType != returnedValue.type())) {
				auto ss = std::stringstream{};
				ss << "bad return type : expected \"" << expectedReturnType << "\" on function declaration but got \"" << returnedValue.type().value() << "\" on return";
				throw std::runtime_error(ss.str());
			}

		} break;
    }
    return true;
}

bool ska::SemanticTypeChecker::matchIfElse(const IfElseTokenEvent& token) {
	const auto& ifElseOperation = OperationType<Operator::IF_ELSE>{token.rootNode()};
	const auto& conditionType = ifElseOperation.GetCondition().type().value_or(Type{});
	if (conditionType != ExpressionType::BOOLEAN) {
		auto ss = std::stringstream{};
		ss << "expression condition is not a boolean (it's a \"" << conditionType << "\")";
		throw std::runtime_error(ss.str());
	}
	return true;
}

bool ska::SemanticTypeChecker::matchArray(const ArrayTokenEvent& token) {

	switch (token.type()) {
	case ArrayTokenEventType::DECLARATION: {
			auto arrayDeclarationOperation = OperationType<Operator::ARRAY_DECLARATION>{token.rootNode()};
			const auto arraySubType = arrayDeclarationOperation.GetArraySubType();
			for (const auto& arrayElementNode : arrayDeclarationOperation) {
				if (arraySubType != arrayElementNode->type()) {
					auto ss = std::stringstream{};
					ss << "array has not uniform types in it : \"" << arraySubType << "\" and \"" << arrayElementNode->type().value() << "\"";
					throw std::runtime_error(ss.str());
				}
			}
		} break;

    case ArrayTokenEventType::EXPRESSION: {
        if(token.rootNode().type() != ExpressionType::ARRAY) {
            auto ss = std::stringstream {}; 
            ss << "expression is not an array (it's a \"" << token.rootNode().type().value_or(Type {}) << "\")";
            throw std::runtime_error(ss.str());
        }
      } break;

	case ArrayTokenEventType::USE:
		break;

	default:
		assert(!"Unhandled event type");
		break;
	}

	return true;
}

bool ska::SemanticTypeChecker::matchFunction(const FunctionTokenEvent& token) {
    switch(token.type()) {
        
        case FunctionTokenEventType::CALL: {
			auto functionCallOperation = OperationType<Operator::FUNCTION_CALL>{ token.rootNode() };
			
            const auto functionFullRequiredType = functionCallOperation.GetFunctionType();
            if(functionFullRequiredType != ExpressionType::FUNCTION) {
				auto ss = std::stringstream{};
				ss << "function \"" << functionFullRequiredType << "\" is called before being declared (or has a bad declaration)";
				throw std::runtime_error(ss.str());
            }

			const auto functionRequiredTypeParameterSize = functionFullRequiredType.size() - 1;
			const auto callNodeParameterSize = functionCallOperation.GetFunctionParameterSize();
            if(functionRequiredTypeParameterSize != callNodeParameterSize) {
                auto ss = std::stringstream {};
                ss << "bad function call : the function \"" << functionFullRequiredType << "\" needs " 
				<< functionRequiredTypeParameterSize << " parameters but is being called with " << callNodeParameterSize 
				<< " parameters";
                throw std::runtime_error(ss.str());
            }

			SLOG(ska::LogLevel::Debug) << functionFullRequiredType << " function has the following arguments types during its call : ";
			auto index = 0u;
			for (auto& arg : functionCallOperation) {		
				const auto calculatedArgumentType = arg->type().value();
				const auto requiredParameterType = functionFullRequiredType.compound()[index];

                if(requiredParameterType.crossTypes(m_typeCrosser, "=", calculatedArgumentType) == ExpressionType::VOID) {
                    auto ss = std::stringstream {};
                    ss << "Type \"" << calculatedArgumentType << "\" is encountered while a type convertible to \"" 
					<< requiredParameterType << "\" is required";
                    throw std::runtime_error(ss.str());
                }
				index++;
            }
        } break;

		case FunctionTokenEventType::DECLARATION_STATEMENT: {
			auto operation = OperationType<Operator::FUNCTION_DECLARATION>{ token.rootNode() };
			auto functionReturnType = operation.GetFunctionPrototype().type().value().compound().back();
			if (functionReturnType != ExpressionType::VOID && 
				!childrenHasReturnOnAllControlPath(operation.GetFunctionBody())) {
				throw std::runtime_error("function lacks of return in one of its code path");
			}
		} break;
		default:
			break;
    }
    
    return true;
}

bool ska::SemanticTypeChecker::matchVariable(const VarTokenEvent& variable) {
	assert(variable.varType().has_value() && "The current variable has no type. Maybe you forgot to add a type builder ?");
	const auto value = variable.value();
    const auto name = variable.name();
	const auto type = variable.varType().value();
    
	SLOG(ska::LogLevel::Debug) << name << " = " << value << ";\tsymbol = " << type;

    if(variable.type() == VarTokenEventType::AFFECTATION) {
		assert(variable.rootNode().size() > 0);
		if (!OperatorTraits::isLvalueCompatible(variable.rootNode()[0].op())) {
			auto ss = std::stringstream{};
			ss << "The symbol \"" << name << "\" is not an lvalue, therefore cannot be assigned";
			throw std::runtime_error(ss.str());
		}
		
		const auto tokenNodeExpressionType = variable.valType().value();
		const auto newTokenType = type.crossTypes(m_typeCrosser, "=", tokenNodeExpressionType);
        if(newTokenType == ExpressionType::VOID) {
            const auto expressionTypeIndex = tokenNodeExpressionType;
			auto ss = std::stringstream{};
			ss << "The symbol \"" << name << "\" has already been declared as \"" <<
				type << "\" but is now wanted to be \"" <<
				expressionTypeIndex << "\"";
			throw std::runtime_error(ss.str());		
        }
		if (OperatorTraits::isNamed(variable.rootNode()[0].op()) && name.empty()) {
			throw std::runtime_error("invalid symbol affectation");
		}
    }

	if (variable.type() == VarTokenEventType::VARIABLE_DECLARATION && 
		OperatorTraits::isNamed(variable.rootNode().op()) && name.empty()) {
		throw std::runtime_error("invalid symbol declaration");
	}

    return true;
}

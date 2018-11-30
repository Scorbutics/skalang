#include "LoggerConfigLang.h"
#include "Parser.h"
#include "OperatorTraits.h"
#include "SymbolTable.h"
#include "LoggerConfigLang.h"
#include "SymbolTableTypeUpdater.h"

SKA_LOGC_CONFIG(ska::LogLevel::Info, ska::SymbolTableTypeUpdater)

ska::SymbolTableTypeUpdater::SymbolTableTypeUpdater(Parser& parser, SymbolTable& symbolTable): 
	m_symbols(symbolTable),
	SubObserver<ExpressionTokenEvent>(std::bind(&SymbolTableTypeUpdater::matchExpression, this, std::placeholders::_1), parser),
	SubObserver<FunctionTokenEvent>(std::bind(&SymbolTableTypeUpdater::matchFunction, this, std::placeholders::_1), parser),
	SubObserver<VarTokenEvent>(std::bind(&SymbolTableTypeUpdater::matchVariable, this, std::placeholders::_1), parser),
	SubObserver<ReturnTokenEvent>(std::bind(&SymbolTableTypeUpdater::matchReturn, this, std::placeholders::_1), parser) {
}

bool ska::SymbolTableTypeUpdater::matchVariable(const VarTokenEvent& event) {
	updateType(event.rootNode());
	return true;
}

bool ska::SymbolTableTypeUpdater::matchExpression(const ExpressionTokenEvent& event) {
	//updateType(event.rootNode());
	return true;
}

bool ska::SymbolTableTypeUpdater::matchReturn(const ReturnTokenEvent & event) {
	//updateType(event.rootNode());
	return true;
}

bool ska::SymbolTableTypeUpdater::matchFunction(const FunctionTokenEvent& event) {
	switch (event.type()) {
	default: break;
		
		case FunctionTokenEventType::DECLARATION_PARAMETERS: {
			for (auto& child : event.rootNode()) {
				if (child->type().has_value()) {
					updateType(*child);
				}
			}
		} break;

	}
	return true;
}

bool ska::SymbolTableTypeUpdater::isOperatorAccepted(const Operator& op) {
	return OperatorTraits::isNamed(op);
}

void ska::SymbolTableTypeUpdater::updateType(const ASTNode& node) {
	/*for (auto& child : node) {
		if (child->type().has_value()) {
			updateType(*child);
		}
	}*/
	
	const auto& type = node.type();
	if (isOperatorAccepted(node.op())) {
		assert(type.has_value() && !node.name().empty());
		auto* symbol = m_symbols[node.name()];	
		assert(symbol != nullptr && !symbol->isCalculated());
			if (symbol->getType() != type.value()) {
				symbol->calculateType(type.value());
				SLOG(LogLevel::Info) << "Type updated for symbol \"" << node.name() << "\" = \"" << node.type().value() << "\"";
			}
			else {
				SLOG(LogLevel::Error) << "No type detected for symbol \"" << node.name() << "\" with operator \"" << node.op() << "\"";
			}
		
	}
}

#include "LoggerConfigLang.h"
#include "Parser.h"
#include "SymbolTable.h"
#include "LoggerConfigLang.h"
#include "SymbolTableTypeUpdater.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::SymbolTableTypeUpdater)

ska::SymbolTableTypeUpdater::SymbolTableTypeUpdater(Parser& parser, SymbolTable& symbolTable): 
	m_symbols(symbolTable),
	SubObserver<ExpressionTokenEvent>(std::bind(&SymbolTableTypeUpdater::matchExpression, this, std::placeholders::_1), parser),
	SubObserver<FunctionTokenEvent>(std::bind(&SymbolTableTypeUpdater::matchFunction, this, std::placeholders::_1), parser),
	SubObserver<VarTokenEvent>(std::bind(&SymbolTableTypeUpdater::matchVariable, this, std::placeholders::_1), parser) {
}

bool ska::SymbolTableTypeUpdater::matchVariable(const VarTokenEvent& event) {
	updateType(event.rootNode());
	return true;
}

bool ska::SymbolTableTypeUpdater::matchExpression(const ExpressionTokenEvent& event) {
	updateType(event.rootNode());
	return true;
}

bool ska::SymbolTableTypeUpdater::matchFunction(const FunctionTokenEvent& event) {
	switch (event.type()) {
	default: break;
		
		case FunctionTokenEventType::DECLARATION_PARAMETERS: {
			updateType(event.rootNode());
		} break;

	}
	return true;
}

void ska::SymbolTableTypeUpdater::updateType(const ASTNode& node) {
	const auto& type = node.type();
	if (node.op().has_value() && node.op().value() != Operator::LITERAL) {
		assert(type.has_value());
		auto* symbol = m_symbols[node.asString()];
		if (symbol != nullptr) {
			symbol->setType(type.value());
			SLOG(LogLevel::Info) << "Type updated for symbol " << node.asString() << " = " << node.type().value().asString();
		} else {
			SLOG(LogLevel::Info) << "No type detected for symbol " << node.asString();
		}
	}
}

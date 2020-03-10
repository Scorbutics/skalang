#include "Config/LoggerConfigLang.h"
#include "Service/StatementParser.h"
#include "NodeValue/OperatorTraits.h"
#include "SymbolTable.h"
#include "SymbolTableUpdater.h"
#include "NodeValue/ScriptAST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::SymbolTableUpdater)

ska::SymbolTableUpdater::SymbolTableUpdater(StatementParser& parser):
	subobserver_priority_queue<VarTokenEvent>(std::bind(&SymbolTableUpdater::matchVariable, this, std::placeholders::_1), parser, 7),
	subobserver_priority_queue<FunctionTokenEvent>(std::bind(&SymbolTableUpdater::matchFunction, this, std::placeholders::_1), parser, 7) {
}

bool ska::SymbolTableUpdater::matchVariable(VarTokenEvent& event) {
	SLOG(LogLevel::Debug) << "Trying to update symbol-type for \"" << event.rootNode().name() << "\" = \"" << event.rootNode().type().value() << "\"";

	switch (event.type()) {
	case VarTokenEventType::AFFECTATION: {
		SLOG(LogLevel::Debug) << "Variable event AFFECTATION";
		auto name = event.rootNode()[0].name();
		if (!name.empty()) {
			updateNode(event.rootNode(), std::move(name), event.script().symbols());
		}
		break;
	}
	case VarTokenEventType::USE:
		SLOG(LogLevel::Debug) << "Variable event USE";
		updateNode(event.rootNode(), event.rootNode().name(), event.script().symbols());
		break;
	
	case VarTokenEventType::FUNCTION_DECLARATION:
	case VarTokenEventType::PARAMETER_DECLARATION:
	case VarTokenEventType::VARIABLE_AFFECTATION:
		SLOG(LogLevel::Debug) << "Variable event DECLARATION";
		updateNode(event.rootNode(), event.name(), event.script().symbols());
		updateType(event.rootNode(), event.script().symbols());
	break;
		
	default:
		throw std::runtime_error("bad var event");
	break;
	}

	return true;
}

bool ska::SymbolTableUpdater::matchFunction(FunctionTokenEvent& event) {
	switch (event.type()) {
	case FunctionTokenEventType::DECLARATION_STATEMENT:
		updateNode(event.rootNode(), event.name(), event.script().symbols());
		break;
	default:
		break;
	}
	return true;
}

void ska::SymbolTableUpdater::updateNode(ASTNode& node, const std::string& variableName, const SymbolTable& symbols) {
	const auto* symbol = symbols[variableName];
	assert(symbol != nullptr);
	node.linkSymbol(*symbol);
}

void ska::SymbolTableUpdater::updateType(const ASTNode& node, SymbolTable& symbols) {
	SLOG(LogLevel::Debug) << "Is operator named : " << OperatorTraits::isNamed(node.op());
	assert(OperatorTraits::isNamed(node.op()));

	const auto& type = node.type();
	assert(type.has_value() && !node.name().empty());
	const auto* symbol = symbols[node.name()];
	if (symbol != nullptr) {
		if (symbol->type() != type.value()) {
			symbols.forceType(node.name(), type.value());
			SLOG(LogLevel::Info) << "Type updated for symbol \"" << node.name() << "\" = \"" << node.type().value() << "\"";
		} else {
			SLOG(LogLevel::Warn) << "No type updated for symbol \"" << node.name() << "\" with operator \"" << node.op() << "\" : existing type \"" << symbol->type() << "\"";
		}
	} else {
		auto ss = std::stringstream{};
		ss << "An error occured in the symbol table context : unknown symbol \"" << node.name() << "\". Cannot update its type.";
		throw std::runtime_error(ss.str());
	}
}

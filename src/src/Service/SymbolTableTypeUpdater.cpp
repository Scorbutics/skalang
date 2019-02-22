#include "Config/LoggerConfigLang.h"
#include "Service/StatementParser.h"
#include "NodeValue/OperatorTraits.h"
#include "SymbolTable.h"
#include "SymbolTableTypeUpdater.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::SymbolTableTypeUpdater)

ska::SymbolTableTypeUpdater::SymbolTableTypeUpdater(StatementParser& parser, SymbolTable& symbolTable):
	m_symbols(symbolTable),
	subobserver_priority_queue<VarTokenEvent>(std::bind(&SymbolTableTypeUpdater::matchVariable, this, std::placeholders::_1), parser, 70) {
}

bool ska::SymbolTableTypeUpdater::matchVariable(const VarTokenEvent& event) {
	switch (event.type()) {
		case VarTokenEventType::AFFECTATION:
		case VarTokenEventType::USE:
			break;
		case VarTokenEventType::FUNCTION_DECLARATION:
		case VarTokenEventType::PARAMETER_DECLARATION:
		case VarTokenEventType::VARIABLE_DECLARATION:
			updateType(event.rootNode());
		break;
		
		default:
			throw std::runtime_error("bad var event");
		break;
	}
	return true;
}

void ska::SymbolTableTypeUpdater::updateType(const ASTNode& node) {
	assert(OperatorTraits::isNamed(node.op()));
		
	const auto& type = node.type();
	assert(type.has_value() && !node.name().empty());
	auto* symbol = m_symbols[node.name()];	
	if (symbol != nullptr) {
		if (symbol->getType() != type.value()) {
			symbol->forceType(type.value());
			SLOG(LogLevel::Info) << "Type updated for symbol \"" << node.name() << "\" = \"" << node.type().value() << "\"";
		} else {
			SLOG(LogLevel::Error) << "No type detected for symbol \"" << node.name() << "\" with operator \"" << node.op() << "\"";
		}
	} else {
		auto ss = std::stringstream{};
		ss << "An error occured in the symbol table context : unknown symbol \"" << node.name() << "\". Cannot update its type.";
		throw std::runtime_error(ss.str());
	}
}

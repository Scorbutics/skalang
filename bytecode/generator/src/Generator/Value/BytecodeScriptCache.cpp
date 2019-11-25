#include "Config/LoggerConfigLang.h"
#include "BytecodeScriptCache.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::ScriptCache);


void ska::bytecode::ScriptCache::setSymbolInfo(const ASTNode& node, SymbolInfo info) {
	if (node.symbol() == nullptr) {
		throw std::runtime_error("Cannot set symbol information for a node without symbol : " + node.name());
	}
	return setSymbolInfo(*node.symbol(), std::move(info));
}

void ska::bytecode::ScriptCache::setSymbolInfo(const Symbol& symbol, SymbolInfo info) {
	SLOG(ska::LogLevel::Debug) << " Setting " << symbol.getName();
	m_symbolInfo[&symbol] = std::move(info);
}

const ska::bytecode::SymbolInfo* ska::bytecode::ScriptCache::getSymbolInfo(const Symbol& symbol) const {
	if(m_symbolInfo.find(&symbol) == m_symbolInfo.end()) {
		return nullptr;
	}
	return &m_symbolInfo.at(&symbol);
}

const ska::bytecode::SymbolInfo* ska::bytecode::ScriptCache::getSymbolInfo(const ASTNode& node) const {
	if(node.symbol() == nullptr) {
		return nullptr;
	}
	return getSymbolInfo(*node.symbol());
}

const std::vector<ska::bytecode::Operand>& ska::bytecode::ScriptCache::getExportedSymbols(std::size_t scriptIndex) {
		SLOG(ska::LogLevel::Info) << "%11cGenerating exported symbols for script \"" << scriptIndex << "\"";
		auto temporarySortedScriptSymbols = std::priority_queue<SymbolWithInfo>{};
		for (const auto& data : m_symbolInfo) {
			if (data.second.exported && data.second.script == scriptIndex) {
				SLOG(ska::LogLevel::Info) << "%11c\tFound symbol " << data.first->getName() << " with info " << data.second;
				temporarySortedScriptSymbols.push(SymbolWithInfo{ data.first, &data.second });
			}
		}

		if (!temporarySortedScriptSymbols.empty()) {
			(*this)[scriptIndex]->setExportedSymbols((*this)[scriptIndex]->origin().generateExportedSymbols(std::move(temporarySortedScriptSymbols)));
		}
	} else {
		SLOG(ska::LogLevel::Info) << "%11cNo generation of exported symbols for script \"" << scriptIndex << "\" required";
	}
	return (*this)[scriptIndex]->exportedSymbols();
}

bool ska::bytecode::ScriptCache::isGenerated(std::size_t index) const { return (*this)[index] != nullptr && !(*this)[index]->empty();}

#include "Config/LoggerConfigLang.h"
#include "BytecodeGenerationOutput.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::GenerationOutput);

ska::bytecode::GenerationOutput::GenerationOutput(ScriptStorage& storage) :
	m_storage(storage) {
}

std::size_t ska::bytecode::GenerationOutput::push(ScriptGenerationService service) {
  const auto index =  m_storage.size();
  auto scriptName = service.program().name();

  SLOG(ska::LogLevel::Debug) << "Getting script generation service for script named \"" << scriptName << "\" at index \"" << index << "\"";

  const auto& [element, isInserted] = m_mapping.emplace(scriptName, index);
  if (!isInserted) {
    throw std::runtime_error("Double insertion of the same script in generation output");
  }
  m_storage.emplace(scriptName, Storage{ std::move(service) });
  return index;
}

void ska::bytecode::GenerationOutput::setOut(std::size_t index, ScriptGenerationOutput scriptOutput) {
  m_storage.resizeIfTooSmall(index + 1);
  m_storage[index]->output = std::move(scriptOutput);
}

std::pair<std::size_t, ska::bytecode::ScriptGenerationService*> ska::bytecode::GenerationOutput::script(const std::string& fullScriptName) {
  auto result = m_mapping.find(fullScriptName);
  if(result == m_mapping.end()) {
    SLOG(ska::LogLevel::Debug) << "No mapping found for script \"" << fullScriptName << "\"";
    return std::make_pair(std::numeric_limits<std::size_t>::max(), nullptr);
  }
  assert(m_storage.size() > result->second);
	const auto isScriptAlreadyGenerated = !m_storage[result->second]->output.empty();
  if(isScriptAlreadyGenerated) {
    SLOG(ska::LogLevel::Debug) << "Script \"" << fullScriptName << "\" was already generated.";
  }

  return std::make_pair(result->second, isScriptAlreadyGenerated ? nullptr : &m_storage[result->second]->service);
}

ska::bytecode::ScriptGenerationService& ska::bytecode::GenerationOutput::script(std::size_t index) {
	return m_storage[index]->service;
}

void ska::bytecode::GenerationOutput::setSymbolInfo(const ASTNode& node, SymbolInfo info) {
	if (node.symbol() == nullptr) {
		throw std::runtime_error("Cannot set symbol information for a node without symbol : " + node.name());
	}
	return setSymbolInfo(*node.symbol(), std::move(info));
}

void ska::bytecode::GenerationOutput::setSymbolInfo(const Symbol& symbol, SymbolInfo info) {
	SLOG(ska::LogLevel::Debug) << " Setting " << symbol.getName();
	m_symbolInfo[&symbol] = std::move(info);
}

const ska::bytecode::SymbolInfo* ska::bytecode::GenerationOutput::getSymbolInfo(const Symbol& symbol) const {
	if(m_symbolInfo.find(&symbol) == m_symbolInfo.end()) {
		return nullptr;
	}
	return &m_symbolInfo.at(&symbol);
}

const ska::bytecode::SymbolInfo* ska::bytecode::GenerationOutput::getSymbolInfo(const ASTNode& node) const {
	if(node.symbol() == nullptr) {
		return nullptr;
	}
	return getSymbolInfo(*node.symbol());
}

const std::vector<ska::bytecode::Value>& ska::bytecode::GenerationOutput::generateExportedSymbols(std::size_t scriptIndex) const {
	if (m_storage[scriptIndex]->output.exportedSymbols().empty()) {
		SLOG(ska::LogLevel::Info) << "%11cGenerating exported symbols for script \"" << scriptIndex << "\"";
		auto temporarySortedScriptSymbols = std::priority_queue<SymbolWithInfo>{};
		for (const auto& data : m_symbolInfo) {
			if (data.second.exported && data.second.script == scriptIndex) {
				SLOG(ska::LogLevel::Info) << "%11c\tFound symbol " << data.first->getName() << " with info " << data.second;
				temporarySortedScriptSymbols.push(SymbolWithInfo{ data.first, &data.second });
			}
		}

		if (!temporarySortedScriptSymbols.empty()) {
			m_storage[scriptIndex]->output.setExportedSymbols(m_storage[scriptIndex]->service.generateExportedSymbols(std::move(temporarySortedScriptSymbols)));
		}
	} else {
		SLOG(ska::LogLevel::Info) << "%11cNo generation of exported symbols for script \"" << scriptIndex << "\" required";
	}
	return m_storage[scriptIndex]->output.exportedSymbols();
}

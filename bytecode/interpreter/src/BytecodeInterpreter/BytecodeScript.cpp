#include "Config/LoggerConfigLang.h"
#include "BytecodeScript.h"

#include "BytecodeInterpreter/BytecodeInterpreter.h"
#include "Runtime/Service/BridgeFunction.h"
#include "Generator/BytecodeGenerator.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::Script);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::Script)
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::Script)
#define LOG_ERROR SLOG_STATIC(ska::LogLevel::Error, ska::bytecode::Script)

ska::bytecode::Operand ska::bytecode::Script::findBytecodeMemoryFromSymbol(const Symbol& symbol) const {
	auto bytecodeSymbol = m_serviceGen.getSymbol(symbol);
	if (bytecodeSymbol.has_value()) {
		throw std::runtime_error("unable to find generated symbol \"" + symbol.name() + "\" in script \"" + astScript().name() + "\"");
	}
	return bytecodeSymbol.value();
}

const ska::Symbol& ska::bytecode::Script::findSymbolFromString(const std::string& key) const {
	const auto& symbols = astScript().symbols();
	auto* symbol = symbols[key];
	if (symbol == nullptr) {
		throw std::runtime_error("unable to find AST symbol \"" + key + "\" in root script \"" + astScript().name() + "\"");
	}
	return *symbol;
}

const ska::Symbol* ska::bytecode::Script::findFieldSymbol(const Symbol* constructor, const BridgeField& field) const {
	if (field.symbol == nullptr) {
		return nullptr;
	}

	if (constructor != nullptr) {
		return (*constructor)(field.symbol->name());
	}

	return &findSymbolFromString(field.symbol->name());
}

void ska::bytecode::Script::fromBridge(BridgeFunction& constructor, ASTNodePtr astRoot, Interpreter& interpreter) {
	m_serviceGen.program().fromBridge(std::move(astRoot));

	LOG_DEBUG << "%14cGenerating bindings for script " << m_serviceGen.name();

	/* Why don't we use the symbol stored directly in constructor.type().symbol() ? */
	/* Because it still targets the template script type, not the bound script one ! */
	/* (remember, one is - almost - the copy of the other) */
	/* so here we have to iterate through the bound script symbols instead of the template one */
	const Symbol* constructorBoundSymbol = !constructor.isVoid() ? &findSymbolFromString(constructor.name()) : nullptr;
	std::size_t bindingId = 0;
	for (const auto& field : constructor.fields()) {
		const Symbol* newerSymbol = findFieldSymbol(constructorBoundSymbol, field);
		if(newerSymbol == nullptr) {
			auto ss = std::stringstream {};
			assert(field.symbol != nullptr);
			ss << "%14cNo symbol attached to type " << field.symbol->type();
			LOG_ERROR << ss.str();
			throw std::runtime_error(ss.str());
		}

		LOG_INFO << "%14cAttaching binding to symbol " << newerSymbol->name();
		auto info = m_cache.getSymbolInfoOrNew(m_serviceGen.id(), *newerSymbol);
		auto bindingRef = ScriptVariableRef{ bindingId++, m_serviceGen.id() };
		m_cache.storeBinding(std::make_shared<NativeFunction>(field.callback), bindingRef);
		info.binding = bindingRef.variable;

		//TODO avoid going through symbol info ?
		// Maybe instead move this algorithm directly to generators
		// and here only store the "constructor" variable somewhere in m_serviceGen
		m_cache.setSymbolInfo(*newerSymbol, std::move(info));
	}

	if (constructorBoundSymbol != nullptr) {
		auto constructorInfo = m_cache.getSymbolInfoOrNew(m_serviceGen.id(), *constructorBoundSymbol);
		auto bindingRef = ScriptVariableRef{ bindingId++, m_serviceGen.id() };
		m_cache.storeBinding(std::make_shared<NativeFunction>([&constructor](std::vector<NodeValue> params) {
			LOG_INFO << "%14cParameters of constructor " << constructor.name() << " : ";
			for (const auto& param : params) {
				LOG_INFO << "%14c" << param.convertString();
			}
			constructor.setAdditionalParams(std::move(params));
			return NodeValue{};
		}, true), bindingRef);
		constructorInfo.binding = bindingRef.variable;
		constructorInfo.bindingPassThrough = true;
		m_cache.setSymbolInfo(*constructorBoundSymbol, std::move(constructorInfo));
	}

	m_serviceGen.generate(m_cache, interpreter.generator());
	LOG_DEBUG << "%14cGeneration done for script " << m_serviceGen.name();
}

std::unique_ptr<ska::bytecode::Executor> ska::bytecode::Script::execute(Interpreter& interpreter) {
	return interpreter.interpret(m_serviceGen.id(), m_cache);
}

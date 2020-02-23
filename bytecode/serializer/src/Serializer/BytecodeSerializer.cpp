#include "Config/LoggerConfigLang.h"
#include <unordered_set>
#include "BytecodeSerializer.h"
#include "BytecodeSerializationContext.h"
#include "BytecodeDeserializationContext.h"

#include "BytecodeScriptHeader.h"
#include "BytecodeScriptBody.h"
#include "BytecodeScriptExternalReferences.h"

static constexpr std::size_t SERIALIZER_VERSION = 1;

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::bytecode::Serializer);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::Serializer)
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::Serializer)
#define LOG_ERROR SLOG_STATIC(ska::LogLevel::Error, ska::bytecode::Serializer)

bool ska::bytecode::Serializer::serialize(SerializationContext& context) const {
	try {
		auto partIndexes = std::deque<std::size_t>{};
		do {
			partIndexes.push_back(context.writeHeader(SERIALIZER_VERSION));

			if (!context.currentScriptBridged()) {
				auto [instructionIndex, linkedScripts] = context.writeInstructions();
				auto exportIndex = context.writeExports();
				partIndexes.push_back(context.writeExternalReferences(std::move(linkedScripts)));
				partIndexes.push_back(instructionIndex);
				partIndexes.push_back(exportIndex);
			}
			
		} while (context.next(std::move(partIndexes)));
		return true;
	} catch (std::runtime_error& e) {
		LOG_ERROR << e.what();
		return false;
	}
}

std::vector<std::string> ska::bytecode::Serializer::deserialize(DeserializationContext& context) const {
	auto failedScripts = std::vector<std::string> {};
	std::string scriptName = context.startScriptName();
	auto scripts = std::unordered_set<std::string> {};
	scripts.insert(scriptName);
	
	auto scriptIt = scripts.begin();
	auto first = true;
	while (scriptIt != scripts.end()) {
		auto canRead = context.read(*scriptIt);
		
		if (canRead) {
			scripts.erase(scriptIt);

			auto script = ScriptParts{};
			LOG_INFO << "Natives section ";
			context >> script.natives;

			context >> script.header;

			LOG_INFO << "[Serializer script version " << script.header.serializerVersion << "], script " << script.header.scriptName() << " refered as id " << script.header.scriptId << " when it was compiled";

			context >> script.references;

			scripts.insert(std::make_move_iterator(script.references.scripts.begin()), std::make_move_iterator(script.references.scripts.end()));
			scriptIt = scripts.begin();
			script.references.scripts.clear();

			if (!script.header.scriptBridged) {
				context >> script.body;
			}
			if (!first) {
				context.declare(script.header.scriptName(), std::move(script.body.instructions), std::move(script.body.exports));
			}
			first = false;
		} else {
			failedScripts.push_back(*scriptIt);
			scriptIt++;
		}
	}
	LOG_INFO << "No more scripts, leaving deserialization";
	return failedScripts;
}

bool ska::bytecode::Serializer::serialize(const ScriptCache& cache, SerializationStrategy output) const {
	auto context = SerializationContext { cache, output };
	return serialize(context);
}

std::vector<std::string> ska::bytecode::Serializer::deserialize(ScriptCache& cache, const std::string& startScriptName, DeserializationStrategy input) const {
	auto context = DeserializationContext{ cache, startScriptName, input };
	return deserialize(context);
}

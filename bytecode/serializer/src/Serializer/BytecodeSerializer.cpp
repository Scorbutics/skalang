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
	} catch (std::runtime_error&) {
		return false;
	}
}

bool ska::bytecode::Serializer::deserialize(DeserializationContext& context) const {
	std::string scriptName = context.startScriptName();
	auto scripts = std::unordered_set<std::string> {};
	auto wasRead = false;
	while (context.read(scriptName)) {
		wasRead = true;
		
		auto script = ScriptParts{};
		LOG_INFO << "Natives section ";
		context >> script.natives;

		context >> script.header;

		LOG_INFO << "[Serializer script version " << script.header.serializerVersion << "], script " << script.header.scriptName() << " refered as id " << script.header.scriptId << " when it was compiled";

		context >> script.references;

		scripts.insert(std::make_move_iterator(script.references.scripts.begin()), std::make_move_iterator(script.references.scripts.end()));
		script.references.scripts.clear();
		scriptName = script.header.scriptName();

		scripts.erase(scriptName);

		if (!script.header.scriptBridged) {
			context >> script.body;
		}

		context.declare(std::move(scriptName), std::move(script.body.instructions), std::move(script.body.exports));

		auto nextScriptIt = scripts.begin();
		if (nextScriptIt == scripts.end()) {
			LOG_INFO << "No more scripts, leaving deserialization";
			break;
		}
		scriptName = *nextScriptIt;
	}
	return wasRead;
}

bool ska::bytecode::Serializer::serialize(const ScriptCache& cache, SerializationStrategy output) const {
	auto context = SerializationContext { cache, output };
	return serialize(context);
}

bool ska::bytecode::Serializer::deserialize(ScriptCache& cache, const std::string& startScriptName, DeserializationStrategy input) const {
	auto context = DeserializationContext{ cache, startScriptName, input };
	return deserialize(context);
}

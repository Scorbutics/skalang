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
		do {
			//TODO : sorting strategy
			auto parts = std::vector<std::stringstream>{};
			context.writeHeader(SERIALIZER_VERSION);

			if (!context.currentScriptBridged()) {
				auto linkedScripts = context.writeInstructions();
				context.writeExports();
				context.writeExternalReferences(std::move(linkedScripts));
				
				parts.push_back(context.pop());
				parts.push_back(context.pop());
				parts.push_back(context.pop());
				
			}
			parts.push_back(context.pop());
			
			context.push(std::move(parts.back()));
			parts.pop_back();

			if (!parts.empty()) {
				context.push(std::move(parts[0]));
				context.push(std::move(parts[2]));
				context.push(std::move(parts[1]));
			}
		} while (context.next());
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
		
		auto header = ScriptHeader{};
		context >> header;

		LOG_INFO << "[Serializer script version " << header.serializerVersion << "], script id " << header.scriptId;

		auto body = ScriptBody{};
		if (!header.scriptBridged) {
			context >> body;
		}

		auto externalReferences = ScriptExternalReferences { header, body };
		context >> externalReferences;

		std::copy(externalReferences.scripts.begin(), externalReferences.scripts.end(), std::inserter(scripts, scripts.end()));
		scriptName = header.scriptName;

		scripts.erase(scriptName);

		LOG_INFO << "[Script name " << scriptName << " with id " << header.scriptId << "]";
		context.declare(header.scriptId, scriptName, std::move(body.instructions), std::move(body.exports));

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

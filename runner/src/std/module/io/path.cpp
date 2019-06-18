#include <cassert>
#include "Config/LoggerConfigLang.h"
#include "path.h"

#include "Service/ReservedKeywordsPool.h"
#include "Service/Tokenizer.h"
#include "Interpreter/Value/Script.h"
#include "Base/IO/Files/FileUtils.h"

ska::lang::IOPathModule::IOPathModule(ModuleConfiguration& config) :
	Module {config, "std.native.io.path"} {
	m_bridge.import(config.parser, config.interpreter, { {"Path", "std:std.io.path"} });
	m_bridge.bindGenericFunction("Build", { "string", "Path::Fcty()" },
    	std::function<ska::NodeValue(Script&, std::vector<ska::NodeValue>)>([&](Script&, std::vector<ska::NodeValue> buildParams) -> ska::NodeValue {
		auto path = m_bridge.callFunction(config.interpreter, "Path", "Fcty", std::move(buildParams));
		auto& memPath = path.nodeval<ska::ObjectMemory>();

		//Query input parameters
		const auto* memberPathValue = (*memPath)["path"].first;
		assert(memberPathValue != nullptr);
		auto pathStr = std::move(memberPathValue->nodeval<std::string>());

		//Build output object
		memPath->emplace("canonical", std::make_unique<ska::BridgeFunction>(
            std::function<ska::NodeValue(Script&, std::vector<ska::NodeValue>)>([&, pathStr(std::move(pathStr))](Script&, std::vector<ska::NodeValue> unused) {
            return FileUtils::getCanonicalPath(pathStr);
        })));

		return ska::NodeValue{ std::move(memPath) };
    }));
	m_bridge.buildFunctions();
}


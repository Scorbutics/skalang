#include <cassert>
#include "Config/LoggerConfigLang.h"
#include "path.h"

#include "Service/ReservedKeywordsPool.h"
#include "Service/Tokenizer.h"
#include "Interpreter/Value/Script.h"
#include "Interpreter/Interpreter.h"
#include "Base/IO/Files/FileUtils.h"
/*
ska::lang::IOPathModule::IOPathModule(ModuleConfiguration& config) :
	Module {config, "std.native.io.path"},
	m_proxy { m_bridge } {
	auto importPath = m_bridge.import(config.parser, config.interpreter, {"Path", "std:std.io.path"});
	m_bridge.bindGenericFunction("Build", { "string", importPath.typeName("Fcty()") },
    	std::function<ska::NodeValue(std::vector<ska::NodeValue>)>([&](std::vector<ska::NodeValue> buildParams) -> ska::NodeValue {
		auto path = m_proxy.callFunction(config.interpreter, "Path", "Fcty", std::move(buildParams));
		auto& memPath = path.nodeval<ska::ObjectMemory>();

		//Query input parameters
		const auto* memberPathValue = (*memPath)["path"].first;
		assert(memberPathValue != nullptr);
		auto pathStr = std::move(memberPathValue->nodeval<StringShared>());

		//Build output object
		memPath->emplace("canonical", std::make_unique<ska::BridgeFunction>(
            std::function<ska::NodeValue(std::vector<ska::NodeValue>)>([&, pathStr(std::move(pathStr))](std::vector<ska::NodeValue> unused) {
            return std::make_shared<std::string>(std::move(FileUtils::getCanonicalPath(*pathStr)));
        })));

		return ska::NodeValue{ std::move(memPath) };
    }));
	m_bridge.buildFunctions();
}

*/
#pragma once
#include "Base/IO/Files/FileUtils.h"

namespace ska {
    
    enum class ScriptNameStrategy {
        WORKING_DIRECTORY,
        RELATIVE_TO_PARENT,
        RELATIVE_TO_RUNNER
    };

    std::string& ScriptNameStandardLibraryPrefix();

    //Execution relative path (based on current working directory)
    inline std::string ScriptNameCurrentWorkingDirectory(const std::string& scriptCalled) {    
	    return FileUtils::getCurrentDirectory() + "/" + scriptCalled + ".miniska";
    }

    //Executable relative path
    inline std::string ScriptNameRelativeToRunnerDirectory(const std::string& scriptCalled) {
        const auto fileNameData = FileNameData {FileUtils::getExecutablePath()};
        return fileNameData.path + "/" + ScriptNameStandardLibraryPrefix() + "/" + scriptCalled + ".miniska";
    }

    //File relative path
    inline std::string ScriptNameRelativeToParentDirectory(const std::string& scriptCaller, const std::string& scriptCalled) {    
	    const auto fileNameData = FileNameData {scriptCaller};
        if(fileNameData.path.empty() || fileNameData.name.empty()) {
            const auto subfileNameData = FileNameData {FileUtils::getExecutablePath()};
            return subfileNameData.path + "/" + scriptCalled + ".miniska";
        }
        return fileNameData.path + "/" + scriptCalled + ".miniska";
    }



    inline std::string ScriptNameDeduce(const std::string& scriptCaller, const std::string& scriptCalled, ScriptNameStrategy empty = ScriptNameStrategy::RELATIVE_TO_PARENT) {
        if(FileUtils::isAbsolutePath(scriptCalled)) {
            return scriptCalled;
        }

        auto strategy = ScriptNameStrategy::RELATIVE_TO_PARENT;
        auto scriptCalledName = std::string{};
        const auto namespaceSeparatorPosition = scriptCalled.find_first_of(':');
        if(namespaceSeparatorPosition != std::string::npos) {
            auto namespace_ = scriptCalled.substr(0, namespaceSeparatorPosition);
            if(namespace_ == "std") {
                strategy = ScriptNameStrategy::RELATIVE_TO_RUNNER;
            } else if(namespace_ == "wd") {
                strategy = ScriptNameStrategy::WORKING_DIRECTORY;
            }
            scriptCalledName = scriptCalled.substr(namespaceSeparatorPosition + 1);
        } else {
            scriptCalledName = scriptCalled;
        }

        switch (strategy) {
        case ScriptNameStrategy::WORKING_DIRECTORY:
            return ScriptNameCurrentWorkingDirectory(scriptCalledName);
        
        case ScriptNameStrategy::RELATIVE_TO_RUNNER:
            return ScriptNameRelativeToRunnerDirectory(scriptCalledName);

        default:
            return ScriptNameRelativeToParentDirectory(scriptCaller, scriptCalledName);
        }
    }
}
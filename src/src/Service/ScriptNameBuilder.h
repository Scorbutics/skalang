#pragma once
#include "Base/IO/Files/FileUtils.h"

namespace ska {
    
    enum class ScriptNameStrategy {
        WORKING_DIRECTORY,
        RELATIVE_TO_PARENT
    };

    //Execution relative path (not file relative path)
    inline std::string ScriptNameCurrentWorkingDirectory(const std::string& scriptName ) {    
	    return FileUtils::getCurrentDirectory() + "/" + scriptName + ".miniska";
    }

    //File relative path
    inline std::string ScriptNameRelativeToParentDirectory(const std::string& scriptCaller, const std::string& scriptCalled) {    
	    auto fileNameData = FileNameData {scriptCaller};
        return fileNameData.path + "/" + scriptCalled + ".miniska";
    }

    inline std::string ScriptNameDeduce(const std::string& scriptCaller, const std::string& scriptCalled, ScriptNameStrategy strategy) {
        switch (strategy) {
        case ScriptNameStrategy::WORKING_DIRECTORY:
            return ScriptNameCurrentWorkingDirectory(scriptCalled);
        
        default:
            return ScriptNameRelativeToParentDirectory(scriptCaller, scriptCalled);
        }
    }
}
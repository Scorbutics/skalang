#include "ScriptNameBuilder.h"

std::string& ska::ScriptNameStandardLibraryPrefix() {
    static std::string prefix = "std";
    return prefix;
}

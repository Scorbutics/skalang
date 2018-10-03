#pragma once
#include <Logging/Logger.h>

namespace ska {
    template <class T>
    class LoggerClassFormatter {
    public:
        static std::string format() {
            const auto classStr = std::string (typeid(T).name());
            const auto lastNamespaceDetected = classStr.rfind("11");
            return lastNamespaceDetected != std::string::npos ? classStr.substr(lastNamespaceDetected + 2) : classStr;
        }
    };
}

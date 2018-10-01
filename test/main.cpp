#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest.h>
#include <Logging/Logger.h>

class LogMinGWGlobalFormatter : 
    public ska::LoggerGlobalFormatter {
public:
    std::string format(const char* className) const override {
        const auto classStr = std::string (className);
        const auto lastNamespaceDetected = classStr.rfind("11");
        return lastNamespaceDetected != std::string::npos ? classStr.substr(lastNamespaceDetected + 2) : classStr;
    }
};

int main() {
    doctest::Context context;
    auto resultCode = 0;

    ska::LoggerFactory::setGlobalFormatter<LogMinGWGlobalFormatter>();

    //context.setOption("order-by", "name");
    //context.setOption("success", "true");
    resultCode = context.run();

    if (context.shouldExit() || resultCode != 0) {
        exit(-1);
    }
}



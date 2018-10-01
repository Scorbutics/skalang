#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest.h>
#include <Logging/Logger.h>

class LogMinGWGlobalFormatter : 
    public LoggerGlobalFormatter {
public:
    std::string format(const char* className) const override {
        return className;
    }
};

int main() {
    doctest::Context context;
    auto resultCode = 0;

    ska::LoggerFactory::setGlobalFormatter<LogMinGWGlobalFormatter>();

    // sort the test cases by their name
    context.setOption("order-by", "name");
    context.setOption("success", "true");
    resultCode = context.run();

    if (context.shouldExit() || resultCode != 0) {
        exit(-1);
    }
}



#include <iostream>
#include <fstream>
#include "LoggerConfigLang.h"
#include <Signal/SignalHandler.h>

detail::SkaLangLogger detail::Logger = detail::BuildLogger("TypeBuilderLogError.txt");

namespace detail {
    template<class T>
        void UpdatePatterns(T& logger) {
            logger.setPattern(ska::LogLevel::Debug, "%10c[%h:%m:%s:%T]%10c[Debug]%8c(%17F l.%4l) %10c%v");
            logger.setPattern(ska::LogLevel::Info, "%10c[%h:%m:%s:%T]%11c[Info ]%8c(%17F l.%4l) %11c%v");
            logger.setPattern(ska::LogLevel::Warn, "%10c[%h:%m:%s:%T]%14c[Warn ]%8c(%17F l.%4l) %14c%v");
            logger.setPattern(ska::LogLevel::Error, "%10c[%h:%m:%s:%T]%12c[Error]%8c(%17F l.%4l) %12c%v");
        }
} 

detail::SkaLangLogger detail::BuildLogger(const char * filename) {
	static auto TypeBuilderLogFileOutput = std::ofstream { filename };
	auto logger = SkaLangLogger{};
	logger.get<0>().addOutputTarget(TypeBuilderLogFileOutput);
	logger.get<1>().addOutputTarget(std::cout);

    UpdatePatterns(logger.get<0>());
    UpdatePatterns(logger.get<1>());
    
    ska::process::SignalHandlerAddAction([](int signalCode) {
        detail::Logger.terminate();
        TypeBuilderLogFileOutput.close();
    });
    
    return logger;
}

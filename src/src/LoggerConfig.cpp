#include <iostream>
#include <fstream>
#include "LoggerConfig.h"
#include <Signal/SignalHandler.h>

detail::SkaLangLogger detail::Logger = detail::BuildLogger("TypeBuilderLogError.txt");

detail::SkaLangLogger detail::BuildLogger(const char * filename) {
	static auto TypeBuilderLogFileOutput = std::ofstream { filename };
	auto logger = SkaLangLogger{};
	logger.get<0>().addOutputTarget(TypeBuilderLogFileOutput);
	logger.get<1>().addOutputTarget(std::cout);
 
	/*logger.get<1>().setPattern(ska::LogLevel::Debug, "%10c[%h:%m:%s:%T]%9c[Debug]%14c(%10F l.%3l) %15c%v");
	logger.get<1>().setPattern(ska::LogLevel::Info, "%10c[%h:%m:%s:%T]%10c[Info ]%14c(%10F l.%3l) %15c%v");
	logger.get<1>().setPattern(ska::LogLevel::Warn, "%10c[%h:%m:%s:%T]%11c[Warn ]%14c(%10F l.%3l) %15c%v");
	logger.get<1>().setPattern(ska::LogLevel::Error, "%10c[%h:%m:%s:%T]%12c[Error]%14c(%10F l.%3l) %15c%v");*/

    ska::process::SignalHandlerAddAction([](int signalCode) {
        detail::Logger.terminate();
        TypeBuilderLogFileOutput.close();
    });
    
    return logger;
}

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
    
    ska::process::SignalHandlerAddAction([](int signalCode) {
        std::cout << "Joining writing thread and closing log file..." << std::endl;
        detail::Logger.terminate();
		TypeBuilderLogFileOutput.close();
        std::cout << "Closed." << std::endl;
    });
    
    return logger;
}

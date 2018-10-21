#include <iostream>
#include <fstream>
#include "LoggerConfig.h"

detail::SkaLangLogger detail::Logger = detail::BuildLogger("TypeBuilderLogError.txt");

detail::SkaLangLogger detail::BuildLogger(const char * filename) {
	static auto TypeBuilderLogFileOutput = std::ofstream { filename };
	auto logger = SkaLangLogger{};
	logger.get<0>().addOutputTarget(TypeBuilderLogFileOutput);
	logger.get<1>().addOutputTarget(std::cout);
	return logger;
}
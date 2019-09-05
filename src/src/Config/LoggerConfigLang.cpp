#include <iostream>
#include <fstream>
#include "LoggerConfigLang.h"
#include <Signal/SignalHandler.h>

ska::detail::SkaLangLogger& ska::detail::LangLogger() {
	static auto logger = ska::detail::BuildLangLogger("Log.txt");
	return logger;
}

namespace ska {
	namespace detail {
		template<class T>
		void UpdatePatterns(T& logger) {
			logger.setPattern(ska::LogLevel::Debug, "%07c[%h:%m:%s:%T]%10c[Debug]%8c(%25F l.%4l) %07c%v");
			logger.setPattern(ska::LogLevel::Info, "%07c[%h:%m:%s:%T]%11c[Info ]%8c(%25F l.%4l) %07c%v");
			logger.setPattern(ska::LogLevel::Warn, "%07c[%h:%m:%s:%T]%14c[Warn ]%8c(%25F l.%4l) %07c%v");
			logger.setPattern(ska::LogLevel::Error, "%07c[%h:%m:%s:%T]%12c[Error]%8c(%25F l.%4l) %07c%v");
			logger.enableComplexLogging();
		}
	}
}
ska::detail::SkaLangLogger ska::detail::BuildLangLogger(const char * filename) {
	static auto TypeBuilderLogFileOutput = std::ofstream { filename };
	auto logger = SkaLangLogger{};
	logger.get<0>().addOutputTarget(TypeBuilderLogFileOutput);
	logger.get<1>().addOutputTarget(std::cout);

	UpdatePatterns(logger.get<0>());
	UpdatePatterns(logger.get<1>());

	ska::process::SignalHandlerAddAction([](int signalCode) {
			detail::LangLogger().terminate();
			TypeBuilderLogFileOutput.close();
	});

	return logger;
}

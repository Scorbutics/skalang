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
			logger.setPattern(ska::LogLevel::Debug, "%10c[D]%8c(%25F l.%3l) %07c%v");
			logger.setPattern(ska::LogLevel::Info, "%11c[I]%8c(%25F l.%3l) %07c%v");
			logger.setPattern(ska::LogLevel::Warn, "%14c[W]%8c(%25F l.%3l) %07c%v");
			logger.setPattern(ska::LogLevel::Error, "%12c[E]%8c(%25F l.%3l) %07c%v");
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

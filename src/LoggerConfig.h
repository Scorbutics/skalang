#pragma once
#include <Logging/Logger.h>
#include <Logging/LogContext.h>

namespace ska {
	
		template <class ... LoggerC>
		class MultiLogger {
			template <std::size_t... Idx>
			static auto make_index_dispatcher(std::index_sequence<Idx...>) {
				return [](auto&& f) { (f(std::integral_constant<std::size_t, Idx>{}), ...); };
			}

			template <std::size_t N>
			static auto make_index_dispatcher() {
				return make_index_dispatcher(std::make_index_sequence<N>{});
			}

			template <typename Tuple, typename Func>
			static void for_each(Tuple&& t, Func&& f) {
				constexpr auto n = std::tuple_size<std::decay_t<Tuple>>::value;
				auto dispatcher = make_index_dispatcher<n>();
				dispatcher([&f, &t](auto idx) { f(std::get<idx>(std::forward<Tuple>(t))); });
			}

		public:
			template <LogLevel logLevel, class Wrapped, long line>
			auto log(char* functionName, char* filename) {
				if constexpr (logLevel >= LoggerClassLevel<Wrapped>::level) {
					return LogEntry{ 
						std::bind(&MultiLogger::onDestroyEntry, this, std::placeholders::_1),
						
						/*[&](LogEntry e) {
							std::apply([&]() { 
								(logForLogger<LoggerC>(std::get<LoggerC>(m_loggers), e), ...); 
							}, m_loggers);
						},*/
						loggerdetail::LogContext { logLevel, LoggerClassFormatter<Wrapped>::className, functionName, filename, line }
					};
				} else {
					return loggerdetail::EmptyProxy{};
				}
			}

			//Direct access for settings
			template <std::size_t index>
			auto& get() {
				return std::get<index>(m_loggers);
			}

		private:
			void onDestroyEntry(const LogEntry& entry) {		
				auto copied = entry;
				copied.resetCallback();
				for_each(m_loggers, [&](auto& logger) {
					using Logger = typename std::remove_reference<decltype(logger)>::type;
					//auto callbackDestroy = std::bind(&Logger::onDestroyEntry, logger, std::placeholders::_1);
					logger.onDestroyEntry(copied);
				});
			}

			/*template <class Logger>
			auto logForLogger(Logger& logger, const LogEntry& entry) {
				logger.onDestroyEntry(entry.cloneMessage(std::bind(&Logger::onDestroyEntry, logger, std::placeholders::_1)));
			}*/

			std::tuple<LoggerC...> m_loggers;
		};
	
}

#define SLOG(multiLogger, level, currentClass) multiLogger.log<level, currentClass, __LINE__>(SKA_CURRENT_FUNCTION, __FILE__ )
#include "Config/LoggerConfigLang.h"
#include "TokenVariant.h"
#include "BytecodeInterpreter/BytecodeExecutionContext.h"
#include "NodeValue.h"
#include "NodeValue/StringShared.h"

 double ska::bytecode::NodeValue::convertNumeric() const {
	double numeric = 0.0;
	const auto& valueVariant = std::get<TokenVariant>(m_variant);
	std::visit([&numeric](auto && arg){
		using T = std::decay_t<decltype(arg)>;

		if constexpr (std::is_same<T, long>::value) {
			numeric = static_cast<double>(arg);
		} else if constexpr (std::is_same<T, double>::value) {
			numeric = arg;
		} else if constexpr (std::is_same<T, bool>::value) {
			numeric = arg ? 1.0 : 0.0;
		} else if constexpr (std::is_same<T, std::size_t>::value) {
			numeric = arg;
		} else if constexpr (std::is_same<T, StringShared>::value) {
			try {
				numeric = std::stod(*arg);
			} catch(std::logic_error& e) {
				auto ss = std::stringstream{};
				ss << "cannot convert the node value \"" << *arg << "\" to a numeric format : " << e.what();
				throw std::runtime_error(ss.str());
			}
		} else {
			throw std::runtime_error("cannot convert the node value to a numeric format");
		}
	}, valueVariant);
	return numeric;
}

std::string ska::bytecode::NodeValue::convertString() const {
	auto result = std::string{};
	const auto& valueVariant = std::get<TokenVariant>(m_variant);
	std::visit([&result](auto && arg) {
		using T = std::decay_t<decltype(arg)>;

		if constexpr (std::is_same<T, long>::value) {
			result = std::to_string(arg);
		} else if constexpr (std::is_same<T, double>::value) {
			result = std::to_string(arg);
		} else if constexpr (std::is_same<T, std::size_t>::value) {
			result = std::to_string(arg);
		} else if constexpr (std::is_same<T, bool>::value) {
			result = arg ? "true" : "false";
		} else if constexpr (std::is_same<T, StringShared>::value) {
			result = *arg;
		} else {
			throw std::runtime_error("cannot convert the node value to a string format");
		}
	}, valueVariant);
	return result;
}

namespace ska {
	namespace bytecode {
		bool operator==(const NodeValue& lhs, const NodeValue& rhs) {
			return lhs.m_variant == rhs.m_variant;
		}
	}
}

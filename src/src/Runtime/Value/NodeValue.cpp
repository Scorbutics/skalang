#include "Config/LoggerConfigLang.h"
#include "Runtime/Value/TokenVariant.h"
#include "NodeValue.h"
#include "StringShared.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::NodeValue);

double ska::NodeValue::convertNumeric() const {
	double numeric = 0.0;
	if(std::holds_alternative<NodeValue*>(m_variant)) {
		return std::get<NodeValue*>(m_variant)->convertNumeric();
	}
	const auto& valueVariant = std::get<TokenVariant>(m_variant);
	std::visit([&numeric](auto && arg){
		using T = std::decay_t<decltype(arg)>;

		if constexpr (std::is_same<T, long>::value) {
			numeric = static_cast<double>(arg);
		} else if constexpr (std::is_same<T, int>::value) {
			numeric = arg;
		} else if constexpr (std::is_same<T, double>::value) {
			numeric = arg;
		} else if constexpr (std::is_same<T, bool>::value) {
			numeric = arg ? 1.0 : 0.0;
		} else if constexpr (std::is_same<T, VariableRef>::value) {
			numeric = arg.variable;
		} else if constexpr (std::is_same<T, ScriptVariableRef>::value) {
			numeric = arg.variable;
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

std::string ska::NodeValue::convertString() const {
	auto result = std::string{};
	if(empty()) {
		return result;
	}

	if(std::holds_alternative<NodeValue*>(m_variant)) {
		return std::get<NodeValue*>(m_variant)->convertString();
	}
	if(std::holds_alternative<TokenVariant>(m_variant)) {
		const auto& valueVariant = std::get<TokenVariant>(m_variant);
		std::visit([&result](auto && arg) {
			using T = std::decay_t<decltype(arg)>;

			if constexpr (std::is_same<T, long>::value) {
				result = std::to_string(arg);
			} else if constexpr (std::is_same<T, int>::value) {
				result = std::to_string(arg);
			} else if constexpr (std::is_same<T, double>::value) {
				result = std::to_string(arg);
			} else if constexpr (std::is_same<T, VariableRef>::value) {
				result = std::to_string(arg.variable);
			} else if constexpr (std::is_same<T, ScriptVariableRef>::value) {
				result = "__script " + std::to_string(arg.script) + ": variable ref " + std::to_string(arg.variable) + "__";
			} else if constexpr (std::is_same<T, bool>::value) {
				result = arg ? "true" : "false";
			} else if constexpr (std::is_same<T, StringShared>::value) {
				result = *arg;
			} else if constexpr (std::is_same_v<T, ObjectMemory> || std::is_same_v<T, NativeFunctionPtr>) {
				result = "__complex memory object__";
			} else {
				throw std::runtime_error("cannot convert the node value to a string format");
			}
		}, valueVariant);
	} else {
		result = "__container__";
	}
	return result;
}

ska::NodeValue& ska::NodeValue::dereference(const NodeValueVariant_& variant) {
	auto& referedObject = *std::get<NodeValue*>(variant);
	if (isReference(referedObject.m_variant)) {
		return dereference(referedObject.m_variant);
	}
	return referedObject;
}

void ska::NodeValue::transferValueToOwned(NodeValueVariant_ arg) {
	if(isReference(m_variant)) {
		SLOG(LogLevel::Debug) << "%11cReference detected, reassigning refered value";
		if (isReference(arg)) {
			dereference(m_variant) = dereference(arg);
		} else {
			dereference(m_variant) = arg;
		}
	} else {
		if(isReference(arg) && std::get<NodeValue*>(arg) == this) {
			SLOG(LogLevel::Error) << "a value ref cannot refer to itself. Aborting assignation.";
			return;
		}
		m_variant = std::move(arg);

		SLOG(LogLevel::Debug) << "%10cAssigning direct value " << convertString();
	}
}

bool ska::NodeValue::isReference(const NodeValueVariant_& arg) {
	return std::holds_alternative<NodeValue*>(arg);
}

namespace ska {
	bool operator==(const NodeValue& lhs, const NodeValue& rhs) {
		return lhs.m_variant == rhs.m_variant;
	}
}

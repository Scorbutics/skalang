#include "Config/LoggerConfigLang.h"
#include "BytecodeOperand.h"
#include "NodeValue/AST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::bytecode::Operand);

ska::bytecode::Operand::Operand(const ASTNode& node) :
	m_type(OperandType::PURE) {

	SLOG(LogLevel::Info) << "Converting \"" << node.name() << "\" to \"" << node.type().value() << "\"";

	switch(node.type().value().type()) {
		case ExpressionType::BOOLEAN:
			m_content = node.name() == "true";
			break;
		case ExpressionType::FLOAT:
			m_content = std::stod(node.name());
		break;
		case ExpressionType::INT:
			m_content = std::stol(node.name());
			break;
		case ExpressionType::STRING:
			m_content = std::make_shared<std::string>(node.name());
			break;
		case ExpressionType::VOID:
			m_type = OperandType::EMPTY;
		break;
		default: {
			auto ss = std::stringstream {};
			ss << "unsupported conversion of node type \"" << node.type().value() << "\"";
			throw std::runtime_error(ss.str());
		}
	}
}

std::string ska::bytecode::Operand::toString() const {
	if(empty()) {
		return "";
	}

	if(std::holds_alternative<StringShared>(m_content)) {
		return *std::get<StringShared>(m_content);
	}

	auto output = std::string {};
	std::visit([&output](const auto& operand) {
		using TypeT = std::decay_t<decltype(operand)>;
		if constexpr (std::is_same_v<ScriptVariableRef, TypeT>) {
			output = std::to_string(operand.variable) /*+ ":" + std::to_string(operand.script)*/;
		} else if constexpr (!std::is_same_v<StringShared, TypeT>) {
			output = std::to_string(operand);
		}
	}, m_content);


	switch(m_type) {
	case OperandType::VAR:
		output = "V" + output;
		break;

	case OperandType::REG:
		output = "R" + output;
		break;

	default:
		break;
	}

	return output;
}

bool ska::bytecode::operator==(const Operand& lhs, const Operand& rhs) {
	return lhs.m_content == rhs.m_content && lhs.m_type == rhs.m_type;
}
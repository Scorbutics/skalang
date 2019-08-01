#include "NodeValue/AST.h"
#include "BytecodeTypeConversion.h"
#include "Generator/Value/BytecodeScript.h"
#include "NodeValue/Type.h"

namespace ska {
	namespace bytecode {
		template <Command cmd1, Command cmd2>
		struct CommandPackOr { static constexpr auto first = cmd1; static constexpr auto second = cmd2; };

		struct TypeConversionData {
      Command command;
			Command commandReverse;
    };
    /*
    using TypeToInstructionConverterCallback = std::function<Command(bool)>;
    using TypeToInstructionConverter = std::variant<Command, TypeToInstructionConverterCallback>;
    */
    using TypeToInstructionConverter = std::vector<TypeConversionData>;
    using TypeToInstructionConverterContainer = std::array<TypeToInstructionConverter, static_cast<std::size_t>(ExpressionType::UNUSED_Last_Length)>;
    using TwoTypesConverterContainer = std::array<TypeToInstructionConverterContainer, static_cast<std::size_t>(ExpressionType::UNUSED_Last_Length)>;
    using TypeConvertToInstructionContainer = std::array<TwoTypesConverterContainer, static_cast<std::size_t>(LogicalOperator::UNUSED_Last_Length)>;

		template <Command cmd>
		struct CommandComplex : std::false_type {};
		template <> struct CommandComplex<Command::CONV_I_D> : std::true_type {};
		template <> struct CommandComplex<Command::CONV_D_I> : std::true_type {};
		template <> struct CommandComplex<Command::CONV_I_STR> : std::true_type {};
		template <> struct CommandComplex<Command::CONV_D_STR> : std::true_type {};

		static void EnqueueConverterItemFunc(TypeConvertToInstructionContainer& container, LogicalOperator op, ExpressionType t1, ExpressionType t2, std::vector<TypeConversionData> value) {
			container[static_cast<std::size_t>(op)][static_cast<std::size_t>(t1)][static_cast<std::size_t>(t2)] = std::move(value);
		}

		template <class Item>
		struct BuildCommandPack {
			static TypeConversionData build() {
				return { Item::first, Item::second };
			}
		};

		template <Command item>
		struct BuildCommand {
			static TypeConversionData build() {
				return { item, item };
			}
		};

		template <Command ... command>
		struct EnqueueConverterItem {
			static_assert(sizeof...(command) <= 2, "Unsupported complex command list (size > 2)");
			static void enqueue(TypeConvertToInstructionContainer& container, LogicalOperator op, ExpressionType t1, ExpressionType t2) {
				EnqueueConverterItemFunc(container, op, t1, t2, { BuildCommand<command>::build()...});
			}
		};

		template <class ... command>
		struct EnqueueConverterItemPack {
			static_assert(sizeof...(command) <= 2, "Unsupported complex command list (size > 2)");
			static void enqueue(TypeConvertToInstructionContainer& container, LogicalOperator op, ExpressionType t1, ExpressionType t2) {
				EnqueueConverterItemFunc(container, op, t1, t2, { BuildCommandPack<command>::build()...});
			}
		};

		static TypeConvertToInstructionContainer BuildTypeConverter() {
			auto result = TypeConvertToInstructionContainer{};

			//Direct types (no conversion)
			EnqueueConverterItem<Command::ADD_I>::enqueue(result, LogicalOperator::ADDITION, ExpressionType::INT, ExpressionType::INT);
			EnqueueConverterItem<Command::ADD_D>::enqueue(result, LogicalOperator::ADDITION, ExpressionType::FLOAT, ExpressionType::FLOAT);
			EnqueueConverterItem<Command::ADD_STR>::enqueue(result, LogicalOperator::ADDITION, ExpressionType::STRING, ExpressionType::STRING);

			EnqueueConverterItem<Command::SUB_I>::enqueue(result, LogicalOperator::SUBSTRACT, ExpressionType::INT, ExpressionType::INT);
			EnqueueConverterItem<Command::SUB_D>::enqueue(result, LogicalOperator::SUBSTRACT, ExpressionType::FLOAT, ExpressionType::FLOAT);
			EnqueueConverterItem<Command::SUB_ARR>::enqueue(result, LogicalOperator::SUBSTRACT, ExpressionType::ARRAY, ExpressionType::ARRAY);

			EnqueueConverterItem<Command::MUL_I>::enqueue(result, LogicalOperator::MULTIPLY, ExpressionType::INT, ExpressionType::INT);
			EnqueueConverterItem<Command::MUL_D>::enqueue(result, LogicalOperator::MULTIPLY, ExpressionType::FLOAT, ExpressionType::FLOAT);

			EnqueueConverterItem<Command::DIV_I>::enqueue(result, LogicalOperator::DIVIDE, ExpressionType::INT, ExpressionType::INT);
			EnqueueConverterItem<Command::DIV_D>::enqueue(result, LogicalOperator::DIVIDE, ExpressionType::FLOAT, ExpressionType::FLOAT);

			//Conversions
			EnqueueConverterItem<Command::CONV_I_STR, Command::ADD_STR>::enqueue(result, LogicalOperator::ADDITION, ExpressionType::INT, ExpressionType::STRING);
			EnqueueConverterItem<Command::CONV_D_STR, Command::ADD_STR>::enqueue(result, LogicalOperator::ADDITION, ExpressionType::FLOAT, ExpressionType::STRING);
			EnqueueConverterItem<Command::CONV_I_D, Command::ADD_D>::enqueue(result, LogicalOperator::ADDITION, ExpressionType::INT, ExpressionType::FLOAT);
			EnqueueConverterItemPack<CommandPackOr<Command::PUSH_F_ARR, Command::PUSH_B_ARR>>::enqueue(result, LogicalOperator::ADDITION, ExpressionType::INT, ExpressionType::ARRAY);
			EnqueueConverterItemPack<CommandPackOr<Command::PUSH_F_ARR_ARR, Command::PUSH_B_ARR_ARR>>::enqueue(result, LogicalOperator::ADDITION, ExpressionType::ARRAY, ExpressionType::ARRAY);

			EnqueueConverterItem<Command::CONV_I_D, Command::SUB_D>::enqueue(result, LogicalOperator::SUBSTRACT, ExpressionType::INT, ExpressionType::FLOAT);
			EnqueueConverterItemPack<CommandPackOr<Command::POP_F_ARR, Command::POP_B_ARR>>::enqueue(result, LogicalOperator::SUBSTRACT, ExpressionType::INT, ExpressionType::ARRAY);

			EnqueueConverterItem<Command::CONV_I_D, Command::MUL_D>::enqueue(result, LogicalOperator::MULTIPLY, ExpressionType::INT, ExpressionType::FLOAT);
			EnqueueConverterItem<Command::MUL_I_ARR>::enqueue(result, LogicalOperator::MULTIPLY, ExpressionType::INT, ExpressionType::ARRAY);
			EnqueueConverterItem<Command::MUL_I_STR>::enqueue(result, LogicalOperator::MULTIPLY, ExpressionType::INT, ExpressionType::STRING);

			EnqueueConverterItem<Command::CONV_I_D, Command::DIV_D>::enqueue(result, LogicalOperator::DIVIDE, ExpressionType::INT, ExpressionType::FLOAT);

			return result;
		}

		static const TypedValueRef& ConvertWhich(const Type& destinationType, const TypedValueRef& node1, const TypedValueRef& node2) {
			if(node1.type != destinationType && node2.type != destinationType) {
				throw std::runtime_error("unable to perform node conversion");
			}

			if(node1.type == node2.type) {
				return node1;
			}
			return node1.type == destinationType ? node2 : node1;
		}

		static std::vector<Command> TypeConversion(LogicalOperator logicalOperator, const Type& valueType, const Type& destinationType, bool reverseOrder) {
			static TypeConvertToInstructionContainer typeConverter = BuildTypeConverter();

			const auto operatorIndex = static_cast<std::size_t>(logicalOperator);
			const auto nodeExpressionTypeIndex = static_cast<std::size_t>(valueType.type());
			const auto resultExpressionTypeIndex = static_cast<std::size_t>(destinationType.type());

			const auto& converterCmd = typeConverter[operatorIndex][nodeExpressionTypeIndex][resultExpressionTypeIndex];
			auto result = std::vector<Command> {};
			if(reverseOrder) {
				for(const auto& cmd : converterCmd) {
					result.push_back(cmd.commandReverse);
				}
			} else {
				for(const auto& cmd : converterCmd) {
					result.push_back(cmd.command);
				}
			}
			return result;
		}
	}
}

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::TypeConversionData);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::TypeConversionData)
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::TypeConversionData)
#define LOG_ERROR SLOG_STATIC(ska::LogLevel::Error, ska::bytecode::TypeConversionData)

ska::bytecode::GenerationOutput ska::bytecode::TypeConversionBinary(Script& script, LogicalOperator logicalOperator, const TypedValueRef& node1, const TypedValueRef& node2, const TypedValueRef& destination) {
	LOG_DEBUG << "Binary operation for nodes " << node1.value.content << " and " << node2.value.content << " with types " << node1.type  << " and " << node2.type;
	const auto& selectedNode = ConvertWhich(destination.type, node1, node2);

	auto result = TypeConversion(logicalOperator, selectedNode.type, destination.type, &selectedNode == &node2);
	if(result.empty()) {
		LOG_ERROR << "No conversion found for nodes " << node1.value.content << " and " << node2.value.content << " with types " << node1.type  << " and " << node2.type;
		return InstructionPack{};
	}

	assert(result.size() == 2 || result.size() == 1);

	if(result.size() == 2) {
		auto tempRegister = script.queryNextRegister(destination.type);
		auto group = GenerationOutput{ { Instruction {result[0], tempRegister, selectedNode.value} } };
		group.push(Instruction { result[1], destination.value, tempRegister, &selectedNode == &node1 ? node2.value : node1.value });
		LOG_INFO << "Conversion detected : " << group.pack();
		return group;
	}

	LOG_DEBUG << "Command output : " << CommandSTR[static_cast<std::size_t>(result[0])];

	return Instruction{result[0], destination.value, node1.value, node2.value };
}


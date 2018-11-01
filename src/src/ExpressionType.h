#pragma once
#include <unordered_map>
#include <vector>
#include <sstream>
#include "LoggerConfigLang.h"

namespace ska {
	struct Type;
}

SKA_LOGC_CONFIG(ska::LogLevel::Info, ska::Type)

namespace ska {

	enum class ExpressionType {
		VOID,
		OBJECT,
		FUNCTION,
		INT,
		FLOAT,
		STRING,
		//??
		ARRAY
	};

	static constexpr const char* ExpressionTypeSTR[] = {
		"void",
		"var",
		"function",
		"int",
		"float",
		"string",
		"array"
	};

	class ScopedSymbolTable;

	struct Type {
		Type() = default;
		//Type(std::string name, ScopedSymbolTable& typeSymbolTable, ExpressionType t) : m_type(std::move(t)), m_symbolTable(&typeSymbolTable), m_alias(std::move(name)) {};
		Type(ExpressionType t) : m_type(std::move(t)) {}

		Type(std::string name, const ScopedSymbolTable& typeSymbolTable, const Type& t) : m_type(t.m_type), m_symbolTable(&typeSymbolTable), m_alias(std::move(name)) {};
        Type(Type&& t) noexcept {
            *this = std::move(t);
        }

        Type(const Type& t) {
            *this = t;
        }

        Type& operator=(const Type& t) {
            m_type = t.m_type;
            m_alias = t.m_alias;
            m_compound = t.m_compound;
            m_symbolTable = t.m_symbolTable;
            m_moved = t.m_moved;
            SLOG(ska::LogLevel::Debug) << "   Copy, Type " << t.asString() << " copied to " << asString();
            return *this;
        }

        Type& operator=(Type&& t) {
			SLOG(ska::LogLevel::Debug) << "   Move, Type " << t.asString();
            m_type = std::move(t.m_type);
            m_alias = std::move(t.m_alias);
            m_compound = std::move(t.m_compound);
            m_symbolTable = std::move(t.m_symbolTable);
            t.m_symbolTable = nullptr; 
            t.m_moved = true;
			SLOG(ska::LogLevel::Debug) << " moved  to " << asString();
            return *this;
        }

        ~Type() = default;

		std::string asString() const {
            if(m_moved) {
                return "INVALID_MOVED";
            }

            if (m_compound.empty()) {
				return m_alias + " " + ExpressionTypeSTR[static_cast<std::size_t>(m_type)];
			}
			auto ss = std::stringstream{}; 
			ss << m_alias << " (" << ExpressionTypeSTR[static_cast<std::size_t>(m_type)];
			for (const auto& childType : m_compound) {
				ss << " - " << childType.asString();
			}
			ss << ")";
			return ss.str();
		}
		
		bool operator==(const Type& t) const {
			if (!m_alias.empty()) {
				return m_alias == t.m_alias;
			}

			if (!t.m_alias.empty()) {
				return false;
			}

			return m_type == t.m_type && m_compound == t.m_compound;
		}

        void name(std::string n) {
			SLOG(ska::LogLevel::Info) << "Naming type " << asString() << " : \"" << n << "\"";
            m_alias = std::move(n);
        }

        const std::string& getName() const {
            return m_alias;
        }

		bool operator==(const ExpressionType& t) const {
			return m_type == t;
		}

		bool operator!=(const ExpressionType& t) const {
			return m_type != t;
		}

		Type& operator=(ExpressionType t) {
			m_type = std::move(t);
			return *this;
		}

		void add(Type t) {
			m_compound.push_back(std::move(t));
		}

		void link(const ScopedSymbolTable& t) {
			m_symbolTable = &t;
		}

		const std::vector<Type>& compound() const {
			return m_compound;
		}

		bool operator!=(const Type& t) const {
			return !(*this == t);
		}

		ExpressionType crossTypes(char op, const Type& type2) const {
			const auto& type1 = m_type;

			constexpr auto TypeMapSize = 7;

			static int typeMapOperatorPlus[TypeMapSize][TypeMapSize] = {
				{ 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 3, 4, 5, 0 },
				{ 0, 0, 0, 4, 4, 0, 0 },
				{ 0, 0, 0, 5, 0, 5, 0 },
				{ 0, 0, 0, 0, 0, 0, 6 }
			};

			static int typeMapOperatorMinus[TypeMapSize][TypeMapSize] = {
				{ 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 3, 4, 0, 0 },
				{ 0, 0, 0, 4, 4, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0 }
			};

			static int typeMapOperatorMul[TypeMapSize][TypeMapSize] = {
				{ 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 3, 4, 5, 0 },
				{ 0, 0, 0, 4, 4, 0, 0 },
				{ 0, 0, 0, 5, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0 }
			};

			static int typeMapOperatorDiv[TypeMapSize][TypeMapSize] = {
				{ 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 3, 4, 0, 0 },
				{ 0, 0, 0, 4, 4, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0 }
			};

			static int typeMapOperatorEqual[TypeMapSize][TypeMapSize] = {
				{ 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 1, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 3, 4, 5, 0 },
				{ 0, 0, 0, 3, 4, 5, 0 },
				{ 0, 0, 0, 3, 4, 5, 0 },
				{ 0, 0, 0, 0, 0, 0, 6 }
			};

			int(*selectedTypeMap)[TypeMapSize];

			switch (op) {
			case '-':
				selectedTypeMap = typeMapOperatorMinus;
				break;
			case '+':
				selectedTypeMap = typeMapOperatorPlus;
				break;
			case '/':
				selectedTypeMap = typeMapOperatorDiv;
				break;
			case '*':
				selectedTypeMap = typeMapOperatorMul;
				break;
			case '=':
				selectedTypeMap = typeMapOperatorEqual;
				break;

			default: {
				/*std::cout << "Unknown operator \"" << op << "\", returning by default first type (of index " <<
					static_cast<std::size_t>(type1) << ") without checking type map" << std::endl;*/
				} return type1;
			}

			const auto typeIdResult = selectedTypeMap[static_cast<std::size_t>(type1)][static_cast<std::size_t>(type2.m_type)];
			if (typeIdResult == 0) {
				auto ss = std::stringstream{};
				ss << "Unable to use operator \"" << op << "\" on types " << ExpressionTypeSTR[static_cast<std::size_t>(type1)] << " and " << ExpressionTypeSTR[static_cast<std::size_t>(type2.m_type)];
				throw std::runtime_error(ss.str());
			}

			return static_cast<ExpressionType>(typeIdResult);
		}
		const ScopedSymbolTable* symbolTable() const {
			return m_symbolTable;
		}

	private:
		ExpressionType m_type = ExpressionType::VOID;
		std::string m_alias;
		std::vector<Type> m_compound;
		const ScopedSymbolTable* m_symbolTable = nullptr;
	    bool m_moved = false;
    };

    static std::unordered_map<std::string, ExpressionType> ExpressionTypeMapBuild() {
        auto result = std::unordered_map<std::string, ExpressionType>{};
        static constexpr auto ExpressionTypeSize = 7;
        for(auto index = 0u; index < ExpressionTypeSize; index++) {
            result[ExpressionTypeSTR[index]] = static_cast<ExpressionType>(index);
        }
        return result;
    }

    static const auto ExpressionTypeMap = ExpressionTypeMapBuild();
}

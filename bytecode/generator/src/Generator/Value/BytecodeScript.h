#pragma once
#include <string>
#include <optional>
#include <unordered_map>

#include "Base/Values/MovableNonCopyable.h"
#include "NodeValue/ScriptAST.h"
#include "BytecodeValue.h"

namespace ska {
	class ASTNode;
	namespace bytecode {

		class UniqueSymbolGetterBase {
			using SymbolUIDContainer = std::unordered_map<const Symbol*, std::size_t>;
		public:
			virtual ~UniqueSymbolGetterBase() = default;

		protected:
			UniqueSymbolGetterBase(char symbol) : m_symbol(symbol) {}

			std::pair<Value, bool> query(const ASTNode& node);
			std::pair<Value, bool> query(const Symbol& symbol);

			SymbolUIDContainer m_container;
			std::size_t m_count = 0;
			char m_symbol = '_';
		};

		template <char SymbolChar>
		class UniqueSymbolGetter :
			public UniqueSymbolGetterBase {

		public:
			UniqueSymbolGetter() : UniqueSymbolGetterBase(SymbolChar) {}
			~UniqueSymbolGetter() override = default;
		};

		class ScriptGenerationService :
			public MovableNonCopyable,
			private UniqueSymbolGetter<'V'> {

			using LabelContainer = std::vector<std::size_t>;
			using VariableGetter = UniqueSymbolGetter<'V'>;
		public:
			ScriptGenerationService(ska::ScriptAST& script);

			ScriptGenerationService(ScriptGenerationService&&) = default;
			ScriptGenerationService& operator=(ScriptGenerationService&&) = default;

			ska::ScriptAST program() { return ska::ScriptAST{ *m_script }; }

			Register queryNextRegister();
			Value querySymbolOrValue(const ASTNode& node);
			Value querySymbol(const Symbol& symbol);

			~ScriptGenerationService() override = default;

		private:
			std::size_t m_register = 0;
			ska::ScriptHandleAST* m_script{};
		};
	}
}

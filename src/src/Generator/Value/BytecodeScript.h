#pragma once
#include <string>
#include <optional>
#include <unordered_map>

#include "Base/Values/MovableNonCopyable.h"
#include "NodeValue/ScriptAST.h"
#include "Generator/Value/BytecodeValue.h"

namespace ska {
	class ASTNode;
	namespace bytecode {

		class UniqueSymbolGetterBase {
			using SymbolUIDContainer = std::unordered_map<const Symbol*, std::size_t>;
		public:
			virtual ~UniqueSymbolGetterBase() = default;

		protected:
			UniqueSymbolGetterBase(char symbol) : m_symbol(symbol) {}

			Value query(const ASTNode& node);

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

		class Script :
			public MovableNonCopyable,
			private UniqueSymbolGetter<'V'>,
			private UniqueSymbolGetter<'L'> {

			using LabelGetter = UniqueSymbolGetter<'L'>;
			using VariableGetter = UniqueSymbolGetter<'V'>;
		public:
			Script(ska::ScriptAST& script);

			Script(Script&&) = default;
			Script& operator=(Script&&) = default;

			ska::ScriptAST program() { return ska::ScriptAST{ *m_script }; }

			Register queryNextRegister();
			Value querySymbolOrValue(const ASTNode& node);
			Value queryLabel(const ASTNode& node);

			~Script() override = default;
		private:
			std::size_t m_register = 0;
			ska::ScriptHandleAST* m_script{};
		};
	}
}

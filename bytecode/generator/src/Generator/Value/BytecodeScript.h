#pragma once
#include <string>
#include <optional>
#include <unordered_map>

#include "Base/Values/MovableNonCopyable.h"
#include "NodeValue/ScriptAST.h"
#include "BytecodeValue.h"
#include "BytecodeSymbolInfo.h"

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

		class Script :
			public MovableNonCopyable,
			private UniqueSymbolGetter<'V'>,
			private UniqueSymbolGetter<'L'> {

			using LabelContainer = std::vector<std::size_t>;
			using LabelGetter = UniqueSymbolGetter<'L'>;
			using VariableGetter = UniqueSymbolGetter<'V'>;
			using SymbolInfosContainer = std::unordered_map<const Symbol*, SymbolInfo>;
		public:
			Script(ska::ScriptAST& script);

			Script(Script&&) = default;
			Script& operator=(Script&&) = default;

			ska::ScriptAST program() { return ska::ScriptAST{ *m_script }; }

			Register queryNextRegister();
			Value querySymbolOrValue(const ASTNode& node);
			Value querySymbol(const Symbol& symbol);
			Value queryLabel(const ASTNode& node);

			void setSymbolInfo(const ASTNode& node, SymbolInfo info);
			const SymbolInfo* getSymbolInfo(const Symbol& symbol) const;
			const SymbolInfo* getSymbolInfo(const ASTNode& node) const;

			~Script() override = default;

		private:
			SymbolInfosContainer m_symbolInfo;
			std::size_t m_register = 0;
			ska::ScriptHandleAST* m_script{};
		};
	}
}

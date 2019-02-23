#pragma once
#include <iostream>
#include <vector>
#include <variant>
#include <memory>

#include "Service/TypeBuilder/TypeBuilderOperator.h"
#include "Service/Script.h"

#include "NodeValue/AST.h"
#include "NodeValue/OperatorTraits.h"
#include "NodeValue/Operator.h"

namespace ska {
	
	class ASTFactory {
	
		template<Operator o>
		static void CheckTokenAssociatedWithOperator(const Token& token) {
			static_assert(o != Operator::BINARY && o != Operator::LITERAL, "Wrong constructor used for a logical ASTNode. Use MakeLogicalNode instead.");
			if constexpr (OperatorTraits::isNamed(o)) {
				assert(!token.empty());
			} else {
				assert(token.empty());
			}
		}
		
		template <class Func, class ... Args>
		static void for_each(Func&& f, Args&& ... t) {
			(f(std::forward<Args>(t)), ...);
		}

		template <class ... Node>
		static std::vector<ASTNodePtr> BuildVectorFromNodePack(std::unique_ptr<Node>&& ... children) {
			auto result = std::vector<ASTNodePtr>{};
			for_each([&result](auto&& n) {
				result.push_back(std::forward<decltype(n)>(n));
			}, std::forward<std::unique_ptr<Node>>(children)...);
			return result;
		}
		
		public:
		template<Operator o, class ... Node>
		static ASTNodePtr MakeNode(std::unique_ptr<Node>&& ... children) {
			return ASTFactory::template MakeNode<o>(Token{}, ASTFactory::template BuildVectorFromNodePack(std::move(children)...));
		}

		template<Operator o, class ... Node>
		static ASTNodePtr MakeNode(Token token, std::unique_ptr<Node>&& ... children) {
			return ASTFactory::template MakeNode<o>(std::move(token), ASTFactory::template BuildVectorFromNodePack(std::move(children)...));
		}

		template<Operator o>
		static ASTNodePtr MakeNode(std::vector<ASTNodePtr> children) {
			return ASTFactory::template MakeNode<o>(Token {}, std::move(children));
		}

		static ASTNodePtr MakeImportNode(Script s) {
			auto node = std::unique_ptr<ASTNode>(new ASTNode(std::move(s)));
			node->m_typeBuilder = std::make_unique<TypeBuilderOperator<Operator::IMPORT>>();
			return node;
		}

		template <Operator o>
		static ASTNodePtr MakeNode(Token token, std::vector<ASTNodePtr> children = std::vector<ASTNodePtr>{}) {
			ASTFactory::template CheckTokenAssociatedWithOperator<o>(token);
			auto node = std::unique_ptr<ASTNode>(new ASTNode(o, std::move(token), std::move(children)));	
			node->m_typeBuilder = std::make_unique<TypeBuilderOperator<o>>();
			return node;
		}

		static ASTNodePtr MakeEmptyNode() {
			return std::unique_ptr<ASTNode>(new ASTNode());
		}

		static ASTNodePtr MakeLogicalNode(Token token, ASTNodePtr l = nullptr, ASTNodePtr r = nullptr) {
			assert(!token.empty());
			return std::unique_ptr<ASTNode>(new ASTNode(std::move(token), std::move(l), std::move(r)));
		}
	};
}

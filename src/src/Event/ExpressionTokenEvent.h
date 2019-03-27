#pragma once

namespace ska {
	class ASTNode;
	class Script;

	class ExpressionTokenEvent {
	public:
		ExpressionTokenEvent(ASTNode& node, Script& s) : node(node), m_script(s) {}

		auto& rootNode() {
			return node;
		}

		const auto& rootNode() const {
			return node;
		}

		Script& script() {
			return m_script;
		}

	private:
		ASTNode& node;
		Script& m_script;
	};
}

#pragma once
#include <cassert>

namespace ska {
    class ASTNode;

    enum class ReturnTokenEventType {
        START,
        BUILTIN,
        OBJECT
    };

    class ReturnTokenEvent {
	public:
        ReturnTokenEvent() : m_type(ReturnTokenEventType::START) {}
		
        template<ReturnTokenEventType type>
        static ReturnTokenEvent Make(ASTNode& node) {
            static_assert(type != ReturnTokenEventType::START);
            return ReturnTokenEvent { node, type };
        }
		
        auto& rootNode() {
			assert(m_node != nullptr);
            return *m_node;
		}

		const auto& rootNode() const {
			assert(m_node != nullptr);
            return *m_node;
		}

		const auto& type() const {
			return m_type;
		}

	private:
        ReturnTokenEvent(ASTNode& node, ReturnTokenEventType type) : m_node(&node), m_type(type) {
        }

		ASTNode* m_node = nullptr;
        ReturnTokenEventType m_type;
    };
}

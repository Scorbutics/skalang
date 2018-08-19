#pragma once

namespace ska {
    class ASTNode;

	enum class VarTokenEventType {
		DECLARATION,
		AFFECTATION,
		USE
	};

    struct VarTokenEvent {
        ASTNode& node;
	VarTokenEventType type = VarTokenEventType::DECLARATION;
    };
}

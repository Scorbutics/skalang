#pragma once

namespace ska {
    class ASTNode;
	
	enum class FunctionTokenEventType {
		DECLARATION,
		CALL
	};

    struct FunctionTokenEvent {
        ASTNode& node;
	FunctionTokenEventType type = FunctionTokenEventType::DECLARATION;
    };
}

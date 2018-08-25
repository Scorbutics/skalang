#pragma once

namespace ska {
    class ASTNode;
	
	enum class FunctionTokenEventType {
		DECLARATION_PARAMETERS,
        DECLARATION_STATEMENT,
		CALL
	};

    struct FunctionTokenEvent {
        std::string name;
        ASTNode& node;
	    FunctionTokenEventType type;
    };
}

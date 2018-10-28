#pragma once

namespace ska {
    	class ASTNode;
	
	enum class BlockTokenEventType {
		START,
		END
	};
    	
	struct BlockTokenEvent {
		BlockTokenEvent(ASTNode& n, BlockTokenEventType t) : 
			node(n), type(t) {
		}

    		ASTNode& node;
		BlockTokenEventType type;
    	};
}

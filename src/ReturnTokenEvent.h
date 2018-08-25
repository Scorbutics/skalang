#pragma once

namespace ska {
    class ASTNode;

    enum class ReturnTokenEventType {
        BUILTIN,
        OBJECT
    };

    struct ReturnTokenEvent {
        ASTNode& node;
        ReturnTokenEventType type;
    };
}

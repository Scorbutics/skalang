#pragma once

#include "NodeValue/Type.h"

namespace ska {
	class Script;
	class ASTNode;

    class TypeBuildUnit {
    public:
        virtual Type build(const Script&, const ASTNode&) = 0;
    };
}

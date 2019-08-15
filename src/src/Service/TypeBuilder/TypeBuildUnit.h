#pragma once

#include "NodeValue/Type.h"

namespace ska {
	class ScriptAST;
	class ASTNode;

    class TypeBuildUnit {
    public:
        virtual Type build(const ScriptAST&, const ASTNode&) = 0;
    };

}

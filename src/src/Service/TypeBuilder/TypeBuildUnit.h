#pragma once

#include "NodeValue/TypeHierarchy.h"

namespace ska {
	class ScriptAST;
	class ASTNode;

    class TypeBuildUnit {
    public:
        virtual TypeHierarchy build(ScriptAST&, const ASTNode&) = 0;
    	virtual ~TypeBuildUnit() = default;
    };

}

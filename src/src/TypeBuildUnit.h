#pragma once

#include "ExpressionType.h"

namespace ska {
    class TypeBuildUnit {
    public:
        virtual Type build() = 0;
    };
}

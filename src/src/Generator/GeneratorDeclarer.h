#pragma once
#include <memory>
#include <vector>

#include "GeneratorOperatorUnit.h"
#include "NodeValue/Operator.h"

namespace ska {
    class BytecodeGenerator;
    template <Operator op, class ... Args>
    static void GeneratorOperatorDeclare(BytecodeGenerator& generator, std::vector<std::unique_ptr<GeneratorOperatorUnit>>& target, Args&& ... args) {
        target[static_cast<std::size_t>(op)] = std::make_unique<GeneratorOperator<op>>(generator, std::forward<Args>(args)...);
    }
}

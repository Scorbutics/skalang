#pragma once

#include <deque>
#include "Token.h"

namespace ska {
    enum class ParsingContextType {
        AFFECTATION,
        FUNCTION_DECLARATION,
        FACTORY_DECLARATION,
        FUNCTION_MEMBER_DECLARATION
    };

    struct ParsingContext {
        ParsingContextType type;
        Token owner;
    };

    using ParsingContextStack = std::deque<ParsingContext>;
}

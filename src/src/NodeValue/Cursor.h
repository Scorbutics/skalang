#pragma once
#include <cstddef>

namespace ska {
  	using LineIndex = unsigned int;
    using ColumnIndex = unsigned int;
    struct Cursor {
        std::size_t rawIndex = 0;
        ColumnIndex column = 1;
        LineIndex line = 1;
    };
}
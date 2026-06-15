#ifndef ITERATE_SQUARE_H
#define ITERATE_SQUARE_H

#include <functional>

namespace IterateSquare {
    using eTileFunc = std::function<bool(int, int)>;
    void iterateSquare(const int k, const eTileFunc& prcs,
                       const int inc = 1);

    using eTileFunc = std::function<bool(int, int)>;
    void iterateDistance(const int k, const eTileFunc& prcs,
                         const int inc = 1);
};

#endif // ITERATE_SQUARE_H

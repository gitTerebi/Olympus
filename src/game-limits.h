#ifndef GAME_LIMITS_H
#define GAME_LIMITS_H

#include "rand.h"

#if (defined (_WIN32) || defined (_WIN64))
    #include <limits.h>
    #include <algorithm>

    namespace std {
        template<class RandomIt>
        void random_shuffle(RandomIt first, RandomIt last)
        {
            typedef typename std::iterator_traits<RandomIt>::difference_type diff_t;

            for (diff_t i = last - first - 1; i > 0; --i)
            {
                using std::swap;
                swap(first[i], first[Rand::rand() % (i + 1)]);
            }
        }
    }
#endif

#endif // GAME_LIMITS_H

#define SDL_MAIN_HANDLED
#include "engine/eresourcetype.h"

#include <cstdio>

namespace
{
    int sFailures = 0;

    void check(const bool ok, const char *const message)
    {
        if (ok)
            return;
        std::printf("FAIL: %s\n", message);
        sFailures++;
    }
}

int main()
{
    check(eResourceTypeHelpers::transportSize(eResourceType::fleece) == 1,
          "fleece cart capacity is 4 loads");
    check(eResourceTypeHelpers::transportSize(eResourceType::fleece, true) == 2,
          "double-capacity fleece cart capacity is 8 loads");

    if (sFailures == 0)
    {
        std::printf("PASS: resource transport size tests\n");
    }
    return sFailures == 0 ? 0 : 1;
}

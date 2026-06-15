#ifndef BUILDABLE_HELPERS_H
#define BUILDABLE_HELPERS_H

class eTileBase;

namespace BuildableHelpers {
    bool canBuildFisheryTR(const eTileBase* const t);
    bool canBuildFisheryBR(const eTileBase* const t);
    bool canBuildFisheryBL(const eTileBase* const t);
    bool canBuildFisheryTL(const eTileBase* const t);
};

#endif // BUILDABLE_HELPERS_H

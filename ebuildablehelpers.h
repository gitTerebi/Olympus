#ifndef EBUILDABLEHELPERS_H
#define EBUILDABLEHELPERS_H

class eTileBase;

namespace eBuildableHelpers {
    bool canBuildFisheryTR(const eTileBase* const t);
    bool canBuildFisheryBR(const eTileBase* const t);
    bool canBuildFisheryBL(const eTileBase* const t);
    bool canBuildFisheryTL(const eTileBase* const t);
};

#endif // EBUILDABLEHELPERS_H

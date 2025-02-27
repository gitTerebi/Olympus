#include "ebuildablehelpers.h"

#include "engine/etilebase.h"

bool tileDry(const eTileBase* const t) {
    if(!t) return false;
    return t->terrain() == eTerrain::dry ||
           t->terrain() == eTerrain::fertile;
}

bool tileWater(const eTileBase* const t) {
    if(!t) return false;
    return t->terrain() == eTerrain::water;
}

bool eBuildableHelpers::canBuildFisheryTR(const eTileBase* const t) {
    if(!t) return false;
    if(!tileDry(t)) return false;

    const auto tr = t->topRight<eTileBase>();
    if(!tr) return false;
    if(!tileWater(tr)) return false;

    const auto trtr = tr->topRight<eTileBase>();
    if(!trtr) return false;
    if(!tileWater(trtr)) return false;
    if(trtr->isShoreTile()) return false;

    const auto trtl = tr->topLeft<eTileBase>();
    if(!trtl) return false;
    if(!tileWater(trtl)) return false;

    const auto trbr = tr->bottomRight<eTileBase>();
    if(!trbr) return false;
    if(!tileWater(trbr)) return false;

    const auto trbrtr = trbr->topRight<eTileBase>();
    if(!trbrtr) return false;
    if(!tileWater(trbrtr)) return false;
    if(trbrtr->isShoreTile()) return false;

    const auto trbrbr = trbr->bottomRight<eTileBase>();
    if(!trbrbr) return false;
    if(!tileWater(trbrbr)) return false;

    const auto br = t->bottomRight<eTileBase>();
    if(!br) return false;
    if(!tileDry(br)) return false;

    return true;
}

bool eBuildableHelpers::canBuildFisheryBR(const eTileBase* const t) {
    if(!t) return false;
    if(!tileDry(t)) return false;

    const auto tr = t->topRight<eTileBase>();
    if(!tr) return false;
    if(!tileDry(tr)) return false;

    const auto trbr = tr->bottomRight<eTileBase>();
    if(!trbr) return false;
    if(!tileWater(trbr)) return false;

    const auto trbrbr = trbr->bottomRight<eTileBase>();
    if(!trbrbr) return false;
    if(!tileWater(trbrbr)) return false;
    if(trbrbr->isShoreTile()) return false;

    const auto trbrtr = trbr->topRight<eTileBase>();
    if(!trbrtr) return false;
    if(!tileWater(trbrtr)) return false;

    const auto br = t->bottomRight<eTileBase>();
    if(!br) return false;
    if(!tileWater(br)) return false;

    const auto brbr = br->bottomRight<eTileBase>();
    if(!brbr) return false;
    if(!tileWater(brbr)) return false;
    if(brbr->isShoreTile()) return false;

    const auto brbl = br->bottomLeft<eTileBase>();
    if(!brbl) return false;
    if(!tileWater(brbl)) return false;

    return true;
}

bool eBuildableHelpers::canBuildFisheryBL(const eTileBase* const t) {
    if(!t) return false;
    if(!tileWater(t)) return false;

    const auto bl = t->bottomLeft<eTileBase>();
    if(!bl) return false;
    if(!tileWater(bl)) return false;
    if(bl->isShoreTile()) return false;

    const auto tl = t->topLeft<eTileBase>();
    if(!tl) return false;
    if(!tileWater(tl)) return false;

    const auto tr = t->topRight<eTileBase>();
    if(!tr) return false;
    if(!tileDry(tr)) return false;

    const auto trbr = tr->bottomRight<eTileBase>();
    if(!trbr) return false;
    if(!tileDry(trbr)) return false;

    const auto br = t->bottomRight<eTileBase>();
    if(!br) return false;
    if(!tileWater(br)) return false;

    const auto brbl = br->bottomLeft<eTileBase>();
    if(!brbl) return false;
    if(!tileWater(brbl)) return false;
    if(brbl->isShoreTile()) return false;

    const auto brbr = br->bottomRight<eTileBase>();
    if(!brbr) return false;
    if(!tileWater(brbr)) return false;

    return true;
}

bool eBuildableHelpers::canBuildFisheryTL(const eTileBase* const t) {
    if(!t) return false;
    if(!tileWater(t)) return false;

    const auto tl = t->topLeft<eTileBase>();
    if(!tl) return false;
    if(!tileWater(tl)) return false;
    if(tl->isShoreTile()) return false;

    const auto bl = t->bottomLeft<eTileBase>();
    if(!bl) return false;
    if(!tileWater(bl)) return false;

    const auto tr = t->topRight<eTileBase>();
    if(!tr) return false;
    if(!tileWater(tr)) return false;

    const auto trtl = tr->topLeft<eTileBase>();
    if(!trtl) return false;
    if(!tileWater(trtl)) return false;
    if(trtl->isShoreTile()) return false;

    const auto trtr = tr->topRight<eTileBase>();
    if(!trtr) return false;
    if(!tileWater(trtr)) return false;

    const auto trbr = tr->bottomRight<eTileBase>();
    if(!trbr) return false;
    if(!tileDry(trbr)) return false;

    const auto br = t->bottomRight<eTileBase>();
    if(!br) return false;
    if(!tileDry(br)) return false;

    return true;
}

#include "ehephaestussanctuary.h"

#include "textures/ebuildingtextures.h"

eHephaestusSanctuary::eHephaestusSanctuary(
        const int sw, const int sh, eGameBoard& board,
        const eCityId cid) :
    eSanctuary(board, eBuildingType::templeHephaestus,
               sw, sh, 60, cid) {

}

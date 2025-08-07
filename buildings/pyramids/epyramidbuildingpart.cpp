#include "epyramidbuildingpart.h"

ePyramidBuildingPart::ePyramidBuildingPart(
        const std::vector<eSanctCost>& cost,
        eGameBoard& board, const int elevation,
        const eCityId cid) :
    ePyramidElement(cost, board,
                    eBuildingType::pyramidPart,
                    elevation, 1, cid) {}

ePyramidBuildingPart::ePyramidBuildingPart(
        ePyramid* const pyramid,
        eGameBoard& board, const int elevation,
        const eCityId cid) :
    ePyramidElement(pyramid, {}, board,
                    eBuildingType::pyramidPart,
                    elevation, 1, cid) {}

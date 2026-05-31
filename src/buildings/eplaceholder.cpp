#include "eplaceholder.h"

ePlaceholder::ePlaceholder(GameBoard& board, const eCityId cid) :
    eBuilding(board, eBuildingType::placeholder, 1, 1, cid) {}

stdsptr<eTexture> ePlaceholder::getTexture(const eTileSize size) const {
    (void)size;
    return nullptr;
}

void ePlaceholder::erase() {}

void ePlaceholder::sanctuaryErase() {
    eBuilding::erase();
}

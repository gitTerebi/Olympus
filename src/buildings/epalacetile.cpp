#include "epalacetile.h"

#include "textures/game-textures.h"
#include "epalace.h"
#include "fileIO/save-archive.h"

ePalaceTile::ePalaceTile(GameBoard& board,
                         const bool other,
                         const eCityId cid) :
    eBuilding(board, eBuildingType::palaceTile, 1, 1, cid),
    mOther(other) {
    GameTextures::loadPalaceTiles();
}

void ePalaceTile::erase() {
    mPalace->erase();
}

std::shared_ptr<Texture> ePalaceTile::getTexture(const eTileSize size) const {
    const int sizeId = static_cast<int>(size);
    const auto& texs = GameTextures::buildings();
    const auto& coll = texs[sizeId].fPalaceTiles;
    const int id = mOther ? 1 : 0;
    return coll.getTexture(id);
}

int ePalaceTile::provide(const eProvide p, const int n) {
    return mPalace->provide(p, n);
}

void ePalaceTile::setPalace(ePalace* const palace) {
    mPalace = palace;
}

ePalace* ePalaceTile::palace() const {
    return mPalace;
}

void ePalaceTile::serializeFields(SaveArchive& ar) {
    eBuilding::serializeFields(ar);
    ar.buildingAsField("palace", &getBoard(), mPalace);
    if(ar.reading()) {
        const stdptr<ePalaceTile> tptr(this);
        ar.addPostFunc([tptr]() {
            if(!tptr) return;
            const auto palace = tptr->palace();
            if(!palace) return;
            palace->addTile(tptr.get());
        }, "ePalaceTile::palace");
    }
}

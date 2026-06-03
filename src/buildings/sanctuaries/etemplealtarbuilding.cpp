#include "etemplealtarbuilding.h"
#include "fileIO/esavearchive.h"

#include "textures/egametextures.h"

#include "sanctuary.h"
#include "engine/game-board.h"

eTempleAltarBuilding::eTempleAltarBuilding(GameBoard& board,
                                           const eCityId cid) :
    eSanctBuilding({{0, 4, 0}}, board,
                   eBuildingType::templeAltar, 2, 2, cid) {
    setOverlayEnabledFunc([this]() {
        const auto s = monument();
        return s->finished();
    });
}

std::vector<eOverlay> eTempleAltarBuilding::getOverlays(const eTileSize size) const {
    const int sizeId = static_cast<int>(size);
    const auto& blds = eGameTextures::buildings()[sizeId];
    const eTextureCollection* coll = nullptr;
    if(mSacrifice == eSacrifice::sheep) {
        eGameTextures::loadAltarSheepOverlay();
        coll = &blds.fAltarSheepOverlay;
    } else if(mSacrifice == eSacrifice::goods) {
        eGameTextures::loadAltarGoodsOverlay();
        coll = &blds.fAltarGoodsOverlay;
    } else if(mSacrifice == eSacrifice::bull) {
        eGameTextures::loadAltarBullOverlay();
        coll = &blds.fAltarBullOverlay;
    } else {
        return {};
    }
    std::vector<eOverlay> result;
    const int frame = textureTime();
    const auto& tex = coll->getTexture(frame % coll->size());
    result.emplace_back(eOverlay{-1.7, -3.4, tex, false});
    return result;
}

void eTempleAltarBuilding::timeChanged(const int by) {
    mSacrificeTime += by;
    if(mSacrificeTime > 25000) {
        mSacrifice = eSacrifice::none;
        if(mSacrificeTime > 100000) {
            eSacrifice s = eSacrifice::goods;
            auto& board = getBoard();
            const auto hasChar = [&](const eCharacterType type) {
                const auto& chars = board.characters();
                for(const auto c : chars) {
                    const auto t = c->type();
                    if(t == type) return true;
                }
                return false;
            };
            if(hasChar(eCharacterType::bull)) {
                s = eSacrifice::bull;
            } else if(hasChar(eCharacterType::sheep)) {
                s = eSacrifice::sheep;
            }
            startSacrifice(s);
        }
    }
}

void eTempleAltarBuilding::serializeFields(eSaveArchive& ar) {
    eSanctBuilding::serializeFields(ar);
    ar.field("id", mId);
    ar.field("sacrifice", mSacrifice);
    ar.field("sacrificeTime", mSacrificeTime);
}

void eTempleAltarBuilding::startSacrifice(const eSacrifice s) {
    mSacrifice = s;
    mSacrificeTime = 0;
}

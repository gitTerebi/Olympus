#include "etemplealtarbuilding.h"
#include "fileIO/esavearchive.h"

#include "textures/game-textures.h"

#include "sanctuary.h"
#include "engine/game-board.h"
#include "characters/priest.h"
#include "characters/actions/priest-sacrifice-action.h"
#include "enumbers.h"

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
    const auto& blds = GameTextures::buildings()[sizeId];
    const eTextureCollection* coll = nullptr;
    if(mSacrifice == eSacrifice::sheep) {
        GameTextures::loadAltarSheepOverlay();
        coll = &blds.fAltarSheepOverlay;
    } else if(mSacrifice == eSacrifice::goods) {
        GameTextures::loadAltarGoodsOverlay();
        coll = &blds.fAltarGoodsOverlay;
    } else if(mSacrifice == eSacrifice::bull) {
        GameTextures::loadAltarBullOverlay();
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
    const int sacrificeTicks = eNumbers::sSacrificeDurationDays * eNumbers::sDayLength;
    if(mSacrifice != eSacrifice::none) {
        mSacrificeTime += by;
        if(mSacrificeTime >= sacrificeTicks) {
            mSacrifice = eSacrifice::none;
            mSacrificeTime = 0;
            if(mOnSacrificeComplete) mOnSacrificeComplete();
        }
        return;
    }

    mSpawnTimer += by;
    const int spawnTicks = eNumbers::sPriestSacrificeRecurringSpawnDays * eNumbers::sDayLength;
    if(mSpawnTimer < spawnTicks) return;
    if(mPriestOut) return;

    const auto s = monument();
    if(!s || !s->finished()) return;

    mSpawnTimer = 0;
    mPriestOut = true;

    auto& board = getBoard();
    const auto cid = cityId();
    const auto c = e::make_shared<Priest>(board);
    c->setBothCityIds(cid);
    c->changeTile(centerTile());
    const auto a = e::make_shared<PriestSacrificeAction>(c.get(), this);
    c->setAction(a);
}

void eTempleAltarBuilding::serializeFields(eSaveArchive& ar) {
    eSanctBuilding::serializeFields(ar);
    ar.field("id", mId);
    ar.field("sacrifice", mSacrifice);
    ar.field("sacrificeTime", mSacrificeTime, 0);
    ar.field("spawnTimer", mSpawnTimer, 0);
    ar.field("priestOut", mPriestOut, false);
}

int eTempleAltarBuilding::sacrificeDaysLeft() const {
    if(mSacrifice == eSacrifice::none) return 0;
    const int total = eNumbers::sSacrificeDurationDays * eNumbers::sDayLength;
    const int left = total - mSacrificeTime;
    return std::max(0, left / eNumbers::sDayLength);
}

void eTempleAltarBuilding::startSacrifice(const eSacrifice s) {
    mSacrifice = s;
    mSacrificeTime = 0;
    mPriestOut = false;
}

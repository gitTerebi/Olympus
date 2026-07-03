#include "etemplealtarbuilding.h"
#include "fileIO/save-archive.h"

#include "textures/game-textures.h"

#include "sanctuary.h"
#include "engine/game-board.h"
#include "characters/priest.h"
#include "characters/actions/priest-sacrifice-action.h"
#include "numbers.h"

eTempleAltarBuilding::eTempleAltarBuilding(GameBoard& board,
                                           const eCityId cid) :
    eSanctBuilding({{0, 4, 0}}, board,
                   eBuildingType::templeAltar, 2, 2, cid) {
    setOverlayEnabledFunc([this]() {
        const auto s = monument();
        return s->finished();
    });
}

std::vector<Overlay> eTempleAltarBuilding::getOverlays(const eTileSize size) const {
    const int sizeId = static_cast<int>(size);
    const auto& blds = GameTextures::buildings()[sizeId];
    const auto s = monument();
    // the altar base sprite is mirrored for odd rotations, so the
    // sacrifice animation has to mirror and shift with it
    const bool flipped = s && (s->rotateId() % 2 == 1);
    const TextureCollection* coll = nullptr;
    switch(mSacrifice) {
    case eSacrifice::sheep:
        GameTextures::loadAltarSheepOverlay();
        coll = flipped ? &blds.fAltarSheepOverlayFlipped :
                         &blds.fAltarSheepOverlay;
        break;
    case eSacrifice::goods:
        GameTextures::loadAltarGoodsOverlay();
        coll = flipped ? &blds.fAltarGoodsOverlayFlipped :
                         &blds.fAltarGoodsOverlay;
        break;
    case eSacrifice::bull:
        GameTextures::loadAltarBullOverlay();
        coll = flipped ? &blds.fAltarBullOverlayFlipped :
                         &blds.fAltarBullOverlay;
        break;
    default:
        return {};
    }
    const int frame = textureTime();
    const auto& tex = coll->getTexture(frame % coll->size());
    // mirror of the even-rotation placement around the altar's center axis
    const double x = flipped ? -1.4 : -1.7;
    const double y = flipped ? -3.7 : -3.4;
    return {Overlay{x, y, tex, false}};
}

void eTempleAltarBuilding::timeChanged(const int by) {
    const int sacrificeTicks = Numbers::sSacrificeDurationDays * Numbers::sDayLength;
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
    const int spawnTicks = Numbers::sPriestSacrificeRecurringSpawnDays * Numbers::sDayLength;
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

void eTempleAltarBuilding::serializeFields(SaveArchive& ar) {
    eSanctBuilding::serializeFields(ar);
    ar.field("id", mId);
    ar.field("sacrifice", mSacrifice);
    ar.field("sacrificeTime", mSacrificeTime, 0);
    ar.field("spawnTimer", mSpawnTimer, 0);
    ar.field("priestOut", mPriestOut, false);
}

int eTempleAltarBuilding::sacrificeDaysLeft() const {
    if(mSacrifice == eSacrifice::none) return 0;
    const int total = Numbers::sSacrificeDurationDays * Numbers::sDayLength;
    const int left = total - mSacrificeTime;
    return std::max(0, left / Numbers::sDayLength);
}

void eTempleAltarBuilding::startSacrifice(const eSacrifice s) {
    mSacrifice = s;
    mSacrificeTime = 0;
    mPriestOut = false;
}

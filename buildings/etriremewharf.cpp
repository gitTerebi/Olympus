#include "etriremewharf.h"

#include "characters/etrireme.h"
#include "characters/actions/etriremeaction.h"
#include "engine/epathfinder.h"
#include "textures/egametextures.h"
#include "engine/egameboard.h"
#include "enumbers.h"

eTriremeWharf::eTriremeWharf(eGameBoard& board,
                             const eDiagonalOrientation o,
                             const eCityId cid) :
    eEmployingBuilding(board, eBuildingType::triremeWharf,
                       3, 3, 100, cid),
    mO(o) {
    eGameTextures::loadTriremeWharf();
    setStashable(eResourceType::wood | eResourceType::armor);
}

eTriremeWharf::~eTriremeWharf() {
    if(mTakeCart) mTakeCart->kill();
    if(mTrireme) mTrireme->kill();
}

std::shared_ptr<eTexture> eTriremeWharf::getTexture(const eTileSize size) const {
    const int sizeId = static_cast<int>(size);
    const auto& blds = eGameTextures::buildings();
    const auto& coll = blds[sizeId].fTriremeWharf;
    auto& board = getBoard();
    const auto dir = board.direction();
    const auto o = sRotated(mO, dir);
    int id = 3;
    switch(o) {
    case eDiagonalOrientation::topRight:
        id = 0;
        break;
    case eDiagonalOrientation::bottomRight:
        id = 1;
        break;
    case eDiagonalOrientation::bottomLeft:
        id = 2;
        break;
    case eDiagonalOrientation::topLeft:
        id = 3;
        break;
    default:
        id = 0;
        break;
    }

    return coll.getTexture(id);
}

std::vector<eOverlay> eTriremeWharf::getOverlays(const eTileSize size) const {
    if(!enabled()) return {};
    const int sizeId = static_cast<int>(size);
    const auto& blds = eGameTextures::buildings()[sizeId];
    auto& board = getBoard();
    const auto dir = board.direction();
    const auto oo = sRotated(mO, dir);
    eOverlay o;
    const eTextureCollection* coll = nullptr;
    if(mTrireme) {
        switch(oo) {
        case eDiagonalOrientation::topRight:
            coll = &blds.fTriremeWharfOverlay1TR;
            o.fX = -0.3;
            o.fY = -3.5;
            break;
        case eDiagonalOrientation::bottomLeft:
            coll = &blds.fTriremeWharfOverlay1BL;
            o.fX = -0.3;
            o.fY = -2.38;
            break;
        default:
        case eDiagonalOrientation::topLeft:
            coll = &blds.fTriremeWharfOverlay1TL;
            o.fX = -0.15;
            o.fY = -2.4;
            break;
        case eDiagonalOrientation::bottomRight:
            coll = &blds.fTriremeWharfOverlay1BR;
            o.fX = 0.25;
            o.fY = -3.4;
            break;
        }
    } else if(mTriremeBuildingTime > 0) {
        switch(oo) {
        case eDiagonalOrientation::topRight:
            coll = &blds.fTriremeWharfOverlay2TR;
            o.fX = 0.2;
            o.fY = -3.5;
            break;
        case eDiagonalOrientation::bottomLeft:
            coll = &blds.fTriremeWharfOverlay2BL;
            o.fX = -0.8;
            o.fY = -3.38;
            break;
        default:
        case eDiagonalOrientation::topLeft:
            coll = &blds.fTriremeWharfOverlay2TL;
            o.fX = -0.75;
            o.fY = -4.0;
            break;
        case eDiagonalOrientation::bottomRight:
            coll = &blds.fTriremeWharfOverlay2BR;
            o.fX = -0.25;
            o.fY = -3.4;
            break;
        }
    }

    if(!coll) return {};

    const int texId = textureTime() % coll->size();
    o.fTex = coll->getTexture(texId);
    return {o};
}

void eTriremeWharf::timeChanged(const int by) {
    if(enabled()) {
        if(!mTakeCart) {
            mTakeCart = spawnCart(eCartActionTypeSupport::take);
            mTakeCart->setMaxDistance(eNumbers::sTriremeWharfMaxResourceTakeDistance);
        }
        if(!mTrireme && mWoodCount > 1 && mArmorCount > 0) {
            mTriremeBuildingTime += by;
            if(mTriremeBuildingTime > eNumbers::sTriremeWharfBuildTime) {
                mTriremeBuildingStage++;
                mTriremeBuildingTime = 0;
                mWoodCount -= 2;
                mArmorCount--;
                if(mTriremeBuildingStage >= eNumbers::sTriremeWharfBuildStages) {
                    mTriremeBuildingStage = 0;
                    mTriremeBuildingTime = 0;
                    spawnTrireme();
                }
            }
        }
    }
    eEmployingBuilding::timeChanged(by);
}

int eTriremeWharf::add(const eResourceType type,
                       const int count) {
    if(type == eResourceType::wood) {
        const int r = std::clamp(mWoodCount + count, 0, mMaxWood);
        const int result = r - mWoodCount;
        mWoodCount = r;
        return result;
    } else if(type == eResourceType::armor) {
        const int r = std::clamp(mArmorCount + count, 0, mMaxArmor);
        const int result = r - mArmorCount;
        mArmorCount = r;
        return result;
    }
    return 0;
}

int eTriremeWharf::count(const eResourceType type) const {
    if(type == eResourceType::wood) {
        return mWoodCount;
    } else if(type == eResourceType::armor) {
        return mArmorCount;
    }
    return 0;
}

int eTriremeWharf::spaceLeft(const eResourceType type) const {
    if(type == eResourceType::wood) {
        return mMaxWood - mWoodCount;
    } else if(type == eResourceType::armor) {
        return mMaxArmor - mArmorCount;
    }
    return 0;
}

std::vector<eCartTask> eTriremeWharf::cartTasks() const {
    std::vector<eCartTask> tasks;

    if(mMaxWood > mWoodCount) {
        eCartTask task;
        task.fType = eCartActionType::take;
        task.fResource = eResourceType::wood;
        task.fMaxCount = mMaxWood - mWoodCount;
        tasks.push_back(task);
    }

    if(mMaxArmor > mArmorCount) {
        eCartTask task;
        task.fType = eCartActionType::take;
        task.fResource = eResourceType::armor;
        task.fMaxCount = mMaxArmor - mArmorCount;
        tasks.push_back(task);
    }

    return tasks;
}

void eTriremeWharf::read(eReadStream& src) {
    eEmployingBuilding::read(src);

    src.readCharacter(&getBoard(), [this](eCharacter* const c) {
        mTakeCart = static_cast<eCartTransporter*>(c);
    });
    src.readCharacter(&getBoard(), [this](eCharacter* const c) {
        mTrireme = static_cast<eTrireme*>(c);
    });
    src >> mWoodCount;
    src >> mArmorCount;
    src >> mTriremeBuildingStage;
    src >> mTriremeBuildingTime;
}

void eTriremeWharf::write(eWriteStream& dst) const {
    eEmployingBuilding::write(dst);

    dst.writeCharacter(mTakeCart);
    dst.writeCharacter(mTrireme);
    dst << mWoodCount;
    dst << mArmorCount;
    dst << mTriremeBuildingStage;
    dst << mTriremeBuildingTime;
}

void eTriremeWharf::triremeCameBack() {
    mAbroad = false;
    spawnTrireme();
}

void eTriremeWharf::triremeLeaving() {
    mAbroad = true;
    mTrireme.clear();
}

bool eTriremeWharf::hasTrireme() const {
    return mTrireme || mAbroad;
}

bool eTriremeWharf::isAtWharf() const {
    if(!mTrireme || mAbroad) return false;
    const auto home = triremeTile();
    const auto spot = mTrireme->tile();
    if(!home || !spot) return false;
    const int dx = home->x() - spot->x();
    const int dy = home->y() - spot->y();
    return dx*dx + dy*dy < 8;
}

eTile *eTriremeWharf::triremeTile() const {
    const auto& under = tilesUnder();
    eTile* startTile = nullptr;
    for(const auto u : under) {
        if(u->hasWater()) {
            startTile = u;
            break;
        }
    }
    if(!startTile) return nullptr;
    eTile* final = nullptr;
    ePathFinder p([&](eTileBase* const t) {
        return t->hasWater();
    }, [&](eTileBase* const t) {
        if(!t->hasDeepWater()) return false;
        final = static_cast<eTile*>(t);
        return true;
    });
    const auto onCid = cityId();
    auto& board = getBoard();
    const auto rect = board.boardCityTileBRect(onCid);
    const int w = board.width();
    const int h = board.height();
    const bool r = p.findPath(rect, startTile, 5, true, w, h);
    if(!r) return nullptr;
    return final;
}

void eTriremeWharf::spawnTrireme() {
    if(mTrireme) return;
    const auto b = e::make_shared<eTrireme>(getBoard());
    b->setBothCityIds(cityId());
    mTrireme = b.get();
    const auto t = triremeTile();
    if(!t) return;
    b->changeTile(t);

    const auto a = e::make_shared<eTriremeAction>(this, b.get());
    b->setAction(a);
}

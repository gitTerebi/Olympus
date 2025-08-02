#include "etriremewharf.h"

#include "characters/etrireme.h"
#include "characters/actions/etriremeaction.h"
#include "characters/actions/ecarttransporteraction.h"
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

bool eTriremeWharf::hasTrireme() const {
    return mTrireme;
}

void eTriremeWharf::spawnTrireme() {
    if(mTrireme) return;
    const auto b = e::make_shared<eTrireme>(getBoard());
    b->setBothCityIds(cityId());
    mTrireme = b.get();
    eTile* t;
    const auto ct = centerTile();
    switch(mO) {
    case eDiagonalOrientation::topRight: {
        const auto tr = ct->topRight<eTile>();
        if(!tr) return;
        t = tr->topRight<eTile>();
    } break;
    case eDiagonalOrientation::bottomLeft:
        t = ct->bottom<eTile>();
        break;
    case eDiagonalOrientation::topLeft:
        t = ct->topLeft<eTile>();
        break;
    default:
    case eDiagonalOrientation::bottomRight: {
        const auto r = ct->right<eTile>();
        if(!r) return;
        t = r->bottomRight<eTile>();
    } break;
    }
    if(!t) return;
    b->changeTile(t);

    const auto a = e::make_shared<eTriremeAction>(this, b.get());
    b->setAction(a);
}

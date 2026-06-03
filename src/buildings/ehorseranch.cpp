#include "ehorseranch.h"

#include "textures/egametextures.h"
#include "fileIO/esavearchive.h"

#include "horse-ranch-enclosure.h"
#include "engine/game-board.h"
#include "enumbers.h"

#include <algorithm>

eHorseRanch::eHorseRanch(GameBoard& board,
                         const eCityId cid) :
    eEmployingBuilding(board, eBuildingType::horseRanch, 3, 3, 15, cid) {
    eGameTextures::loadHorseRanch();
}

eHorseRanch::~eHorseRanch() {
    if(mTakeCart) mTakeCart->kill();
}

void eHorseRanch::erase() {
    if(mEnclosure) mEnclosure->eBuilding::erase();
    eBuilding::erase();
}

std::shared_ptr<eTexture> eHorseRanch::getTexture(
        const eTileSize size) const {
    const int sizeId = static_cast<int>(size);
    const auto& blds = eGameTextures::buildings();
    return blds[sizeId].fHorseRanch;
}

std::vector<eOverlay> eHorseRanch::getOverlays(
        const eTileSize size) const {
    const int sizeId = static_cast<int>(size);
    const auto& blds = eGameTextures::buildings();
    const auto& texs = blds[sizeId];
    const auto& coll = texs.fHorseRanchOverlay;
    const int texId = textureTime() % coll.size();

    std::vector<eOverlay> os;
    eOverlay& o = os.emplace_back();
    o.fTex = coll.getTexture(texId);
    o.fX = -2.1;
    o.fY = -2.85;

    if(mWheat > 0) {
        const int nw = std::clamp((mWheat - 50)/100, 0, 7);
        eOverlay& wo = os.emplace_back();
        wo.fTex = texs.fWaitingWheat.getTexture(nw);
        wo.fX = -0.4;
        wo.fY = -2.7;
    }

    return os;
}

void eHorseRanch::timeChanged(const int by) {
    if(enabled()) {
        if(!mTakeCart) {
            mTakeCart = spawnCart(eCartActionTypeSupport::get);
        }
        if(mWheat > 0) {
            mWheatTime += by;
            if(mWheatTime > eNumbers::sHorseRanchWheatUsePeriod) {
                mWheatTime = 0;
                mWheat -= 10;
            }
            mHorseTime += by;
            if(mHorseTime > eNumbers::sHorseRanchHorseSpawnPeriod) {
                mHorseTime = 0;
                if(mEnclosure) mEnclosure->spawnHorse();

                const bool isPp = isPersonPlayer();
                if(isPp) {
                    auto& board = getBoard();
                    board.incProduced(eResourceType::horse, 1);
                }
            }
        }
    }
    eEmployingBuilding::timeChanged(by);
}

int eHorseRanch::count(const eResourceType type) const {
    if(type == eResourceType::wheat) return mWheat/100;
    if(type == eResourceType::horse) return horseCount();
    return eEmployingBuilding::count(type);
}

int eHorseRanch::add(const eResourceType type, const int count) {
    if(type == eResourceType::wheat) {
        mWheat += count*100;
        return count;
    }
    return eEmployingBuilding::add(type, count);
}

int eHorseRanch::take(const eResourceType type, const int count) {
    if(type == eResourceType::horse) {
        return mEnclosure ? mEnclosure->take(type, count) : 0;
    }
    return eEmployingBuilding::take(type, count);
}

int eHorseRanch::spaceLeft(const eResourceType type) const {
    if(type == eResourceType::wheat) return 8 - mWheat/100;
    return eEmployingBuilding::spaceLeft(type);
}

std::vector<eCartTask> eHorseRanch::cartTasks() const {
    auto tasks = eEmployingBuilding::cartTasks();

    if(800 > mWheat) {
        eCartTask task;
        task.fType = eCartActionType::get;
        task.fResource = eResourceType::wheat;
        task.fMaxCount = 8 - mWheat/100;
        tasks.push_back(task);
    }

    return tasks;
}

int eHorseRanch::provide(const eProvide p, const int n) {
    if(mEnclosure) mEnclosure->eBuildingWithResource::provide(p, n);
    return eEmployingBuilding::provide(p, n);
}

void eHorseRanch::setEnclosure(HorseRanchEnclosure* const e) {
    mEnclosure = e;
}

int eHorseRanch::horseCount() const {
    return mEnclosure ? mEnclosure->horseCount() : 0;
}

bool eHorseRanch::takeHorse() {
    return mEnclosure ? mEnclosure->takeHorse() : false;
}

void eHorseRanch::serializeFields(eSaveArchive& ar) {
    eEmployingBuilding::serializeFields(ar);
    ar.field("wheat", mWheat);
    ar.field("wheatTime", mWheatTime);
    ar.field("horseTime", mHorseTime);
    ar.characterField("takeCart", &getBoard(), mTakeCart);
    ar.buildingAsField("enclosure", &getBoard(), mEnclosure);
}

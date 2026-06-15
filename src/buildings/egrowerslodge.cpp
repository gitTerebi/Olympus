#include "egrowerslodge.h"

#include "textures/game-textures.h"
#include "characters/actions/cart-transporter-action.h"
#include "characters/actions/egroweraction.h"
#include "buildings/eresourcebuilding.h"
#include "engine/game-board.h"
#include "numbers.h"
#include "fileIO/save-archive.h"

#include <algorithm>
#include <memory>

eGrowersLodge::eGrowersLodge(GameBoard& board, const eGrowerType type,
                             const eCityId cid) :
    eEmployingBuilding(board,
                       type == eGrowerType::grapesAndOlives ?
                           eBuildingType::growersLodge :
                           eBuildingType::orangeTendersLodge, 2, 2, 12,
                       cid),
    mType(type) {
    switch(type) {
    case eGrowerType::grapesAndOlives:
        GameTextures::loadGrowersLodge();
        break;
    case eGrowerType::oranges:
        GameTextures::loadOrangeTendersLodge();
        break;
    }
}

eGrowersLodge::~eGrowersLodge() {
    killWalkers();
}

void eGrowersLodge::erase() {
    killWalkers();
    eBuilding::erase();
}

void eGrowersLodge::killWalkers() {
    if(mCart) {
        mCart->kill();
        mCart.clear();
    }
    if(mGrower) {
        mGrower->kill();
        mGrower.clear();
    }
    for(const auto& h : mOliveHarvesters) {
        if(h) h->kill();
    }
    for(auto& h : mOliveHarvesters) {
        h.clear();
    }
}

std::shared_ptr<Texture> eGrowersLodge::getTexture(const eTileSize size) const {
    const int sizeId = static_cast<int>(size);
    const auto& texs = GameTextures::buildings()[sizeId];
    switch(mType) {
    case eGrowerType::grapesAndOlives:
        return texs.fGrowersLodge;
    case eGrowerType::oranges:
        return texs.fOrangeTendersLodge;
    }
    return nullptr;
}

std::vector<Overlay> eGrowersLodge::
    getOverlays(const eTileSize size) const {
    const int sizeId = static_cast<int>(size);
    const auto& texs = GameTextures::buildings()[sizeId];
    std::vector<Overlay> os;

    switch(mType) {
    case eGrowerType::grapesAndOlives: {
        const auto& coll = texs.fGrowersLodgeOverlay;
        const int texId = textureTime() % coll.size();
        Overlay o;
        o.fTex = coll.getTexture(texId);
        o.fX = -1.45;
        o.fY = -2.2;
        os.push_back(o);
    } break;
    case eGrowerType::oranges: {
        const auto& coll = texs.fOrangeTendersLodgeOverlay;
        const int texId = textureTime() % coll.size();
        Overlay o;
        o.fTex = coll.getTexture(texId);
        o.fX = -1.75;
        o.fY = -2.3;
        os.push_back(o);
    } break;
    }

    const int grapeLoads = mGrapes / sUnitsPerLoad;
    const int oliveLoads = mOlives / sUnitsPerLoad;
    const int orangeLoads = mOranges / sUnitsPerLoad;
    if(grapeLoads > 0) {
        Overlay grapes;
        const auto& coll = texs.fWaitingGrapes;
        const int resMax = coll.size() - 1;
        const int res = std::clamp(grapeLoads - 1, 0, resMax);
        grapes.fTex = coll.getTexture(res);
        grapes.fX = -0.2;
        grapes.fY = -1.7;
        os.push_back(grapes);
    }
    if(oliveLoads > 0) {
        Overlay olives;
        const auto& coll = texs.fWaitingOlives;
        const int resMax = coll.size() - 1;
        const int res = std::clamp(oliveLoads - 1, 0, resMax);
        olives.fTex = coll.getTexture(res);
        olives.fX = -0.5;
        olives.fY = -1.85;
        os.push_back(olives);
    }
    if(orangeLoads > 0) {
        Overlay oranges;
        const auto& coll = texs.fWaitingOranges;
        const int resMax = coll.size() - 1;
        const int res = std::clamp(orangeLoads - 1, 0, resMax);
        oranges.fTex = coll.getTexture(res);
        oranges.fX = 0.00;
        oranges.fY = -1.80;
        os.push_back(oranges);
    }
    return os;
}

void eGrowersLodge::timeChanged(const int by) {
    if(enabled()) {
        if(!mCart) {
            mCart = spawnCart(eCartActionTypeSupport::deliver);
        }
        if(mCart) {
            mCart->setMaxDistance(Numbers::sResourceBuildingMaxResourceGiveDistance);
        }
        if(mSpawnEnabled) {
            const int readyOlives = readyOliveCount();
            const int readyGrapes = readyGrapeCount();
            const int readyOranges = readyOrangeCount();
            const int oliveSpace = spaceLeft(eResourceType::olives);
            const int grapeSpace = spaceLeft(eResourceType::grapes);
            const int orangeSpace = spaceLeft(eResourceType::oranges);
            const bool canOliveHarvest =
                mType == eGrowerType::grapesAndOlives &&
                oliveSpace > 0 &&
                readyOlives > 0;
            const bool canGrapeHarvest =
                mType == eGrowerType::grapesAndOlives &&
                grapeSpace > 0 &&
                readyGrapes > 0 &&
                isGrapeHarvestMonth();
            const bool canOrangeHarvest =
                mType == eGrowerType::oranges &&
                orangeSpace > 0 &&
                readyOranges > 0 &&
                isOrangeHarvestMonth();
            const bool canHarvest =
                canOliveHarvest || canGrapeHarvest || canOrangeHarvest;
            const bool needGrower = !mGrower;
            int harvesters = 0;
            for(const auto& h : mOliveHarvesters) {
                if(h) harvesters++;
            }
            const int fieldCap = 4;
            auto aliveCount = [&]() {
                return (mGrower ? 1 : 0) + harvesters;
            };
            const double eff = effectiveness();
            const double waitMax = Numbers::sGrowerSpawnWaitTime;
            const bool needHarvester =
                canHarvest && harvesters < fieldCap;
            if(needHarvester) {
                for(int i = 0; i < static_cast<int>(mOliveHarvesters.size()); i++) {
                    auto& h = mOliveHarvesters[i];
                    auto& spawnTime = mOliveHarvesterSpawnTimes[i];
                    if(h) continue;
                    spawnTime += by*eff;
                    if(spawnTime <= waitMax) continue;
                    if(aliveCount() >= fieldCap) {
                        spawnTime = waitMax;
                        continue;
                    }
                    const bool ok = canOliveHarvest ?
                        spawnGrower(h, true) :
                        spawnGrower(h, false, true);
                    if(ok) {
                        spawnTime = 0;
                        harvesters++;
                    }
                }
            }
            const bool fruitRipe = canHarvest;
            if(needGrower) {
                mGrowerSpawnTime += by*eff;
                if(mGrowerSpawnTime > waitMax) {
                    const bool blocked = aliveCount() >= fieldCap ||
                                         fruitRipe;
                    if(!blocked && spawnGrower(mGrower)) {
                        mGrowerSpawnTime = 0;
                    } else {
                        mGrowerSpawnTime = waitMax;
                    }
                }
            }
        }
    }
    eEmployingBuilding::timeChanged(by);
}

int eGrowersLodge::add(const eResourceType type, const int count) {
    int* bucket = nullptr;
    if(type == eResourceType::olives) bucket = &mOlives;
    else if(type == eResourceType::grapes) bucket = &mGrapes;
    else if(type == eResourceType::oranges) bucket = &mOranges;
    else return 0;
    const int unitSpace = sMaxUnits - *bucket;
    const int loadSpace = unitSpace / sUnitsPerLoad;
    const int r = std::clamp(count, 0, loadSpace);
    *bucket += r * sUnitsPerLoad;
    return r;
}

int eGrowersLodge::take(const eResourceType type, const int count) {
    int* bucket = nullptr;
    if(type == eResourceType::olives) bucket = &mOlives;
    else if(type == eResourceType::grapes) bucket = &mGrapes;
    else if(type == eResourceType::oranges) bucket = &mOranges;
    else return 0;
    const int loads = *bucket / sUnitsPerLoad;
    const int result = std::clamp(count, 0, loads);
    *bucket -= result * sUnitsPerLoad;
    return result;
}

int eGrowersLodge::count(const eResourceType type) const {
    if(type == eResourceType::olives) return mOlives / sUnitsPerLoad;
    if(type == eResourceType::grapes) return mGrapes / sUnitsPerLoad;
    if(type == eResourceType::oranges) return mOranges / sUnitsPerLoad;
    return 0;
}

int eGrowersLodge::spaceLeft(const eResourceType type) const {
    switch(mType) {
    case eGrowerType::grapesAndOlives:
        if(type == eResourceType::olives) {
            return (sMaxUnits - mOlives) / sUnitsPerLoad;
        }
        if(type == eResourceType::grapes) {
            return (sMaxUnits - mGrapes) / sUnitsPerLoad;
        }
        break;
    case eGrowerType::oranges:
        if(type == eResourceType::oranges) {
            return (sMaxUnits - mOranges) / sUnitsPerLoad;
        }
        break;
    }
    return 0;
}

std::vector<eCartTask> eGrowersLodge::cartTasks() const {
    std::vector<eCartTask> tasks;

    const int grapeLoads = mGrapes / sUnitsPerLoad;
    const int oliveLoads = mOlives / sUnitsPerLoad;
    const int orangeLoads = mOranges / sUnitsPerLoad;

    if(grapeLoads > 0) {
        eCartTask task;
        task.fType = eCartActionType::deliver;
        task.fResource = eResourceType::grapes;
        task.fMaxCount = grapeLoads;
        tasks.push_back(task);
    }

    if(oliveLoads > 0) {
        eCartTask task;
        task.fType = eCartActionType::deliver;
        task.fResource = eResourceType::olives;
        task.fMaxCount = oliveLoads;
        tasks.push_back(task);
    }

    if(orangeLoads > 0) {
        eCartTask task;
        task.fType = eCartActionType::deliver;
        task.fResource = eResourceType::oranges;
        task.fMaxCount = orangeLoads;
        tasks.push_back(task);
    }

    return tasks;
}

void eGrowersLodge::nextMonth() {
    mRingIdx = (mRingIdx + 1) % 12;
    mProducedThisYear -= mMonthlyProduced[mRingIdx];
    mMonthlyProduced[mRingIdx] = 0;
    if(mProducedThisYear < 0) mProducedThisYear = 0;
    const auto curMonth = getBoard().date().month();
    const bool startGrape = mType == eGrowerType::grapesAndOlives &&
                            curMonth == eMonth::october;
    const bool startOlive = mType == eGrowerType::grapesAndOlives &&
                            curMonth == eMonth::january;
    const bool startOrange = mType == eGrowerType::oranges &&
                             curMonth == eMonth::january;
    if(startGrape || startOlive || startOrange) {
        for(auto& t : mOliveHarvesterSpawnTimes) {
            t = Numbers::sGrowerSpawnWaitTime;
        }
    }
}

void eGrowersLodge::growerDelivered(const eResourceType type, const int count) {
    if(count <= 0) return;
    int* bucket = nullptr;
    if(type == eResourceType::olives) bucket = &mOlives;
    else if(type == eResourceType::grapes) bucket = &mGrapes;
    else if(type == eResourceType::oranges) bucket = &mOranges;
    else return;
    const int unitSpace = sMaxUnits - *bucket;
    const int added = std::clamp(count, 0, unitSpace);
    *bucket += added;
    mProducedThisYear += added;
    mMonthlyProduced[mRingIdx] += added;
}

void eGrowersLodge::serializeFields(SaveArchive& ar) {
    eEmployingBuilding::serializeFields(ar);
    ar.field("noTarget", mNoTarget);
    ar.field("spawnEnabled", mSpawnEnabled);
    ar.field("grapes", mGrapes);
    ar.field("olives", mOlives);
    ar.field("oranges", mOranges);
    ar.characterAsField("cart", &getBoard(), mCart);
    ar.field("spawnTime", mSpawnTime);
    ar.field("growerSpawnTime", mGrowerSpawnTime, mSpawnTime);
    ar.characterAsField("grower", &getBoard(), mGrower);
    ar.countedArrayField(
        "oliveHarvesters",
        static_cast<int>(mOliveHarvesters.size()),
        [this](SaveArchive& itemAr, const int i) {
            if(i >= 0 && i < static_cast<int>(mOliveHarvesters.size())) {
                itemAr.characterAsField("harvester", &getBoard(), mOliveHarvesters[i]);
            } else {
                auto ignored = std::make_shared<stdptr<eGrower>>();
                itemAr.characterAsField("harvester", &getBoard(), *ignored);
                itemAr.addPostFunc([ignored]() {}, "eGrowersLodge::ignoredHarvester");
            }
        });
    for(int i = 0; i < static_cast<int>(mOliveHarvesterSpawnTimes.size()); i++) {
        ar.field(("oliveHarvesterSpawnTime." + std::to_string(i)).c_str(),
                 mOliveHarvesterSpawnTimes[i], mSpawnTime);
    }
    ar.field("producedThisYear", mProducedThisYear);
    for(int i = 0; i < 12; i++) {
        ar.field(("monthlyProduced." + std::to_string(i)).c_str(),
                 mMonthlyProduced[i]);
    }
    ar.field("ringIdx", mRingIdx);
}


bool eGrowersLodge::hasReadyOlives() const {
    return readyOliveCount() > 0;
}

int eGrowersLodge::readyOliveCount() const {
    const auto m = getBoard().date().month();
    const bool harvestMonth = m == eMonth::january ||
                              m == eMonth::february ||
                              m == eMonth::march;
    if(!harvestMonth) return 0;
    int result = 0;
    for(const auto b : getBoard().buildings()) {
        if(!b || b->cityId() != cityId()) continue;
        if(b->type() != eBuildingType::oliveTree) continue;
        const auto rb = static_cast<eResourceBuilding*>(b);
        if(rb->resource() > 0) result++;
    }
    return result;
}

int eGrowersLodge::readyGrapeCount() const {
    if(!isGrapeHarvestMonth()) return 0;
    int result = 0;
    for(const auto b : getBoard().buildings()) {
        if(!b || b->cityId() != cityId()) continue;
        if(b->type() != eBuildingType::vine) continue;
        const auto rb = static_cast<eResourceBuilding*>(b);
        if(rb->resource() > 0) result++;
    }
    return result;
}

int eGrowersLodge::readyOrangeCount() const {
    if(!isOrangeHarvestMonth()) return 0;
    int result = 0;
    for(const auto b : getBoard().buildings()) {
        if(!b || b->cityId() != cityId()) continue;
        if(b->type() != eBuildingType::orangeTree) continue;
        const auto rb = static_cast<eResourceBuilding*>(b);
        if(rb->resource() > 0) result++;
    }
    return result;
}

bool eGrowersLodge::isGrapeHarvestMonth() const {
    const auto m = getBoard().date().month();
    return m == eMonth::october ||
           m == eMonth::november ||
           m == eMonth::december;
}

bool eGrowersLodge::isOrangeHarvestMonth() const {
    const auto m = getBoard().date().month();
    return m == eMonth::january ||
           m == eMonth::february ||
           m == eMonth::march;
}

bool eGrowersLodge::spawnGrower(stdptr<eGrower>& grower,
                                const bool oliveHarvester,
                                const bool genericHarvester) {
    const auto t = centerTile();
    const auto g = e::make_shared<eGrower>(getBoard());
    g->setGrowerType(mType);
    g->setBothCityIds(cityId());
    g->changeTile(t);
    const auto mode = oliveHarvester ? eGrowerActionMode::oliveHarvester :
        (genericHarvester ? eGrowerActionMode::normal :
        (mType == eGrowerType::grapesAndOlives ?
             eGrowerActionMode::oliveGroomer :
             eGrowerActionMode::normal));
    const auto a = e::make_shared<eGrowerAction>(
                       mType, this, g.get(), mode);
    g->setAction(a);
    grower = g.get();
    return true;
}

void eGrowersLodge::setNoTarget(const bool t) {
    mNoTarget = t;
}

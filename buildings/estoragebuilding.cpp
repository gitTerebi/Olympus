#include "estoragebuilding.h"
#include "fileIO/esavearchive.h"

#include "engine/e-game-board.h"
#include "engine/eboardcity.h"
#include "characters/ecarttransporter.h"
#include "characters/actions/ecarttransporteraction.h"
#include "characters/actions/storage-delivery-cart.h"

eStorageBuilding::eStorageBuilding(eGameBoard& board,
                                   const eBuildingType type,
                                   const int sw, const int sh,
                                   const int maxEmployees,
                                   const eResourceType canAccept,
                                   const eCityId cid,
                                   const int spaceCount) :
    eEmployingBuilding(board, type, sw, sh, maxEmployees, cid),
    mCanAccept(canAccept & board.supportedResources(cid)),
    mSpaceCount(spaceCount) {
    const auto all = eResourceTypeHelpers::extractResourceTypes(mCanAccept);
    for(const auto a : all) {
        if(a == eResourceType::sculpture) {
            mMaxCount[a] = mSpaceCount;
        } else {
            mMaxCount[a] = 4*mSpaceCount;
        }
    }
    board.registerStorBuilding(this);
    setStashable(mCanAccept);
}

eStorageBuilding::~eStorageBuilding() {
    if(mCart1) mCart1->kill();
    if(mCart2) mCart2->kill();
    getBoard().unregisterStorBuilding(this);
}

void eStorageBuilding::timeChanged(const int by) {
    eEmployingBuilding::timeChanged(by);
    if(enabled()) {
        const auto tasks = cartTasks();
        bool hasDeliverWork = false;
        bool hasGetWork = false;
        for(const auto& task : tasks) {
            if(task.fType == eCartActionType::deliver) hasDeliverWork = true;
            if(task.fType == eCartActionType::get) hasGetWork = true;
        }
        const auto idleAndEmpty = [](eCartTransporter* const c) {
            if(!c) return false;
            if(c->hasResource()) return false;
            const auto act = dynamic_cast<eCartTransporterAction*>(c->action());
            if(!act) return false;
            return act->state() == eCartState::idle;
        };
        const auto isDeliver = [](eCartTransporter* const c) {
            return c && c->support() == eCartActionTypeSupport::deliver;
        };
        const auto isGet = [](eCartTransporter* const c) {
            return c && c->support() == eCartActionTypeSupport::get;
        };
        const auto killCart = [](stdptr<eCartTransporter>& c) {
            if(c) c->kill();
            c = nullptr;
        };
        const auto putCart = [this](const stdptr<eCartTransporter>& c) {
            if(!mCart1) mCart1 = c;
            else if(!mCart2) mCart2 = c;
        };
        const auto activeCount = [this]() {
            int result = 0;
            if(mCart1) result++;
            if(mCart2) result++;
            return result;
        };
        const auto deliveryCount = [this, &isDeliver]() {
            int result = 0;
            if(isDeliver(mCart1.get())) result++;
            if(isDeliver(mCart2.get())) result++;
            return result;
        };
        const auto getCount = [this, &isGet]() {
            int result = 0;
            if(isGet(mCart1.get())) result++;
            if(isGet(mCart2.get())) result++;
            return result;
        };
        const auto spawnDeliverCart = [this]() {
            if(type() == eBuildingType::warehouse) {
                return spawnStorageDeliveryCart();
            }
            return spawnCart(eCartActionTypeSupport::deliver);
        };

        if(!hasDeliverWork && idleAndEmpty(mCart1.get()) && isDeliver(mCart1.get())) killCart(mCart1);
        if(!hasDeliverWork && idleAndEmpty(mCart2.get()) && isDeliver(mCart2.get())) killCart(mCart2);
        if(!hasGetWork && idleAndEmpty(mCart1.get()) && isGet(mCart1.get())) killCart(mCart1);
        if(!hasGetWork && idleAndEmpty(mCart2.get()) && isGet(mCart2.get())) killCart(mCart2);
        if(hasDeliverWork && deliveryCount() == 0) {
            if(activeCount() >= 2 && idleAndEmpty(mCart1.get()) && isGet(mCart1.get())) killCart(mCart1);
            if(activeCount() >= 2 && idleAndEmpty(mCart2.get()) && isGet(mCart2.get())) killCart(mCart2);
        }

        if(hasDeliverWork && deliveryCount() == 0 && activeCount() < 2) {
            putCart(spawnDeliverCart());
        }

        const int getSlots = hasGetWork ? (hasDeliverWork ? 1 : 2) : 0;
        while(getCount() < getSlots && activeCount() < 2) {
            putCart(spawnCart(eCartActionTypeSupport::get));
        }
    }
}

stdptr<eCartTransporter> eStorageBuilding::spawnStorageDeliveryCart() {
    const auto t = centerTile();
    auto& board = getBoard();

    const auto c = e::make_shared<eCartTransporter>(board);
    c->setBothCityIds(cityId());
    c->setAtlantean(atlantean());
    c->changeTile(t);
    c->setVisible(false);

    const auto a = e::make_shared<eStorageDeliveryCartAction>(c.get(), this);
    c->setAction(a);
    c->setSupport(eCartActionTypeSupport::deliver);
    return c.get();
}

int eStorageBuilding::addNotAccept(const eResourceType type, const int count) {
    const bool sculpt = type == eResourceType::sculpture;
    const int sspace = sculpt ? 1 : 4;
    const int max = mMaxCount[type];
    const int ccount = sCount(type, mResourceCount, mResource, mSpaceCount);
    int added = 0;
    const int maxRem = std::max(0, max - ccount);
    int rem = std::min(maxRem, count);
    for(int i = 0; i < mSpaceCount && rem > 0; i++) {
        const auto t = mResource[i];
        int& c = mResourceCount[i];
        if(t == type) {
            const int dep = std::min(sspace - c, rem);
            rem -= dep;
            c += dep;
            added += dep;
        }
    }
    for(int i = 0; i < mSpaceCount && rem > 0; i++) {
        auto& t = mResource[i];
        int& c = mResourceCount[i];
        if(t == eResourceType::none) {
            t = type;
            const int dep = std::min(sspace, rem);
            rem -= dep;
            c += dep;
            added += dep;
        }
    }
    return added;
}

int eStorageBuilding::add(const eResourceType type, const int count) {
    if(!static_cast<bool>(mAccept & type)) return 0;
    return addNotAccept(type, count);
}

int eStorageBuilding::take(const eResourceType type, const int count) {
    int rem = count;
    const int dep = takeFromStash(type, rem);
    rem -= dep;
    if(rem <= 0) return count;
    for(int i = 0; i < mSpaceCount && rem > 0; i++) {
        auto& t = mResource[i];
        int& c = mResourceCount[i];
        if(static_cast<bool>(t & type)) {
            const int dep = std::min(c, rem);
            rem -= dep;
            c -= dep;
            if(c == 0) {
                t = eResourceType::none;
            }
        }
    }
    addFromStash();
    return count - rem;
}

int eStorageBuilding::count(const eResourceType type) const {
    return sCount(type, mResourceCount, mResource, mSpaceCount) +
            stashCount(type);
}

int eStorageBuilding::spaceLeft(const eResourceType type) const {
    return sSpaceLeft(type, mResourceCount, mResource,
                      mAccept, mMaxCount, mSpaceCount);
}


int eStorageBuilding::spaceLeftDontAccept(const eResourceType type) const {
    return sSpaceLeftDontAccept(type, mResourceCount, mResource,
                                mMaxCount, mSpaceCount);
}

std::vector<eCartTask> eStorageBuilding::cartTasks() const {
    auto tasks = orderCartTasks();
    if(type() == eBuildingType::granary ||
       type() == eBuildingType::tradePost) return tasks;
    const auto pushTasks = this->pushCartTasks();
    tasks.insert(tasks.end(), pushTasks.begin(), pushTasks.end());
    return tasks;
}

std::vector<eCartTask> eStorageBuilding::orderCartTasks() const {
    std::vector<eCartTask> tasks;
    const auto gets = eResourceTypeHelpers::extractResourceTypes(mGet);
    const auto empties = eResourceTypeHelpers::extractResourceTypes(mEmpty);
    const auto& board = getBoard();
    const auto city = board.boardCityWithId(cityId());
    for(const auto g : gets) {
        if(city && city->isStockpiled(g)) continue;
        const int space = spaceLeft(g);
        if(space > 0 && getTargetExists(g)) {
            eCartTask task;
            task.fType = eCartActionType::get;
            task.fResource = g;
            task.fMaxCount = space;
            tasks.push_back(task);
        }
    }

    for(const auto e : empties) {
        const int c = count(e);
        if(c > 0 && deliveryTargetExists(e, true)) {
            eCartTask task;
            task.fType = eCartActionType::deliver;
            task.fResource = e;
            task.fMaxCount = c;
            tasks.push_back(task);
        }
    }

    if(type() != eBuildingType::granary &&
       type() != eBuildingType::tradePost) {
        const auto& stash = this->stash();
        for(const auto& s : stash) {
            if(city && city->isStockpiled(s.fType)) continue;
            const bool g = static_cast<bool>(mGet & s.fType);
            if(g) continue;
            if(!deliveryTargetExists(s.fType, true)) continue;
            eCartTask task;
            task.fType = eCartActionType::deliver;
            task.fResource = s.fType;
            task.fMaxCount = s.fCount;
            tasks.push_back(task);
        }
    }
    return tasks;
}

std::vector<eCartTask> eStorageBuilding::pushCartTasks() const {
    std::vector<eCartTask> tasks;
    const auto& board = getBoard();
    const auto city = board.boardCityWithId(cityId());

    // push held stock to needy consumers; limit one cart at a time on push.
    const auto deliveryOut = [](eCartTransporter* const c) {
        return c &&
               c->support() == eCartActionTypeSupport::deliver &&
               c->hasResource();
    };
    if(!deliveryOut(mCart1.get()) && !deliveryOut(mCart2.get())) {
        for(int i = 0; i < mSpaceCount; i++) {
            const auto t = mResource[i];
            const int c = mResourceCount[i];
            if(c <= 0 || t == eResourceType::none) continue;
            if(city && city->isStockpiled(t)) continue;
            if(static_cast<bool>(mGet & t)) continue;
            if(static_cast<bool>(mEmpty & t)) continue;
            if(!deliveryTargetExists(t, false)) continue;
            eCartTask task;
            task.fType = eCartActionType::deliver;
            task.fResource = t;
            task.fMaxCount = c;
            task.fStoragePush = true;
            tasks.push_back(task);
        }
    }
    return tasks;
}

bool eStorageBuilding::getTargetExists(const eResourceType res) const {
    const auto cid = cityId();
    const auto& board = getBoard();
    const auto targets = board.buildings(cid, [this, res, &board](
                                         eBuilding* const b) {
        if(!b) return false;
        if(b == this) return false;
        const auto city = board.boardCityWithId(b->cityId());
        if(city && city->isStockpiled(res)) return false;
        const auto rb = dynamic_cast<eBuildingWithResource*>(b);
        if(!rb) return false;
        return rb->count(res) > 0;
    });
    return !targets.empty();
}

bool eStorageBuilding::deliveryTargetExists(
        const eResourceType res,
        const bool allowStorageTargets) const {
    const auto cid = cityId();
    const auto& board = getBoard();
    const auto targets = board.buildings(cid, [this, res, cid, &board,
                                               allowStorageTargets](
                                              eBuilding* const b) {
        if(!b) return false;
        if(b == this) return false;

        const auto type = b->type();
        if(type == eBuildingType::tradePost) return false;
        const bool storageTarget = type == eBuildingType::warehouse ||
                                   type == eBuildingType::granary;
        if(storageTarget && !allowStorageTargets) return false;

        const auto rb = dynamic_cast<eBuildingWithResource*>(b);
        if(!rb) return false;

        const auto storage = dynamic_cast<eStorageBuilding*>(rb);
        if(storage && storage->empties(res)) return false;

        const int reserved = incomingReservedFor(b, res, board, cid);
        return rb->spaceLeft(res) - reserved > 0;
    });
    return !targets.empty();
}

int eStorageBuilding::incomingReservedFor(const eBuilding* target,
                                          eResourceType res,
                                          const eGameBoard& board,
                                          eCityId cid) {
    if(!target) return 0;
    const auto city = board.boardCityWithId(cid);
    if(!city) return 0;
    int total = 0;
    for(const auto y : city->storBuildings()) {
        if(!y) continue;
        for(eCartTransporter* const cart : {y->cart1(), y->cart2()}) {
            if(!cart) continue;
            if(!cart->hasResource()) continue;
            if(cart->resType() != res) continue;
            const auto act = dynamic_cast<eCartTransporterAction*>(
                                 cart->action());
            if(!act) continue;
            if(act->target() != target) continue;
            total += cart->resCount();
        }
    }
    return total;
}

int eStorageBuilding::sCount(const eResourceType type,
                             const int resourceCount[15],
                             const eResourceType resourceType[15],
                             const int spaceCount) {
    int result = 0;
    for(int i = 0; i < spaceCount; i++) {
        const auto t = resourceType[i];
        if(static_cast<bool>(t & type)) {
            result += resourceCount[i];
        }
    }
    return result;
}

int eStorageBuilding::sSpaceLeftDontAccept(
        const eResourceType type,
        const int resourceCount[15],
        const eResourceType resourceType[15],
        const std::map<eResourceType, int>& maxCounts,
        const int spaceCount) {
    const bool sculpt = type == eResourceType::sculpture;
    const int sspace = sculpt ? 1 : 4;
    int space = 0;
    int count = 0;
    for(int i = 0; i < spaceCount; i++) {
        const int c = resourceCount[i];
        const auto t = resourceType[i];
        if(c == 0) {
            space += sspace;
        } else if(static_cast<bool>(t & type)) {
            space += sspace - c;
            count += c;
        }
    }
    int max;
    if(eResourceTypeHelpers::isSingleType(type)) {
        max = maxCounts.at(type);
    } else {
        max = 0;
        for(const auto& m : maxCounts) {
            if(static_cast<bool>(type & m.first)) {
                max += m.second;
            }
        }
    }

    return std::min(std::max(0, max - count), space);
}

int eStorageBuilding::sSpaceLeft(
        const eResourceType type,
        const int resourceCount[15],
        const eResourceType resourceType[15],
        const eResourceType accepts,
        const std::map<eResourceType, int>& maxCounts,
        const int spaceCount) {
    if(!static_cast<bool>(accepts & type)) return 0;
    return sSpaceLeftDontAccept(type, resourceCount,
                                resourceType, maxCounts,
                                spaceCount);
}

void eStorageBuilding::setMaxCount(const std::map<eResourceType, int>& m) {
    for(const auto& r : m) {
        mMaxCount[r.first] = r.second;
    }
}

void eStorageBuilding::setOrders(const eResourceType get,
                                 const eResourceType empty,
                                 const eResourceType accept) {
    mGet = get;
    mEmpty = empty;
    mAccept = accept | get;
}

void eStorageBuilding::getOrders(eResourceType& get,
                                 eResourceType& empty,
                                 eResourceType& accept) const {
    get = mGet;
    empty = mEmpty;
    accept = mAccept;
}

void eStorageBuilding::read(eReadStream& src) {
    eEmployingBuilding::read(src);
    eSaveArchive ar(src);
    serialize(ar);
}

void eStorageBuilding::write(eWriteStream& dst) const {
    eEmployingBuilding::write(dst);
    eSaveArchive ar(dst);
    const_cast<eStorageBuilding*>(this)->serialize(ar);
}

void eStorageBuilding::serialize(eSaveArchive& ar) {
    ar.field("get", mGet);
    ar.field("empty", mEmpty);
    ar.field("accept", mAccept);
    for(int i = 0; i < 15; i++) {
        ar.field(("resourceCount." + std::to_string(i)).c_str(), mResourceCount[i]);
    }
    for(int i = 0; i < 15; i++) {
        ar.field(("resource." + std::to_string(i)).c_str(), mResource[i]);
    }

    int maxCountCount = static_cast<int>(mMaxCount.size());
    ar.field("maxCount.count", maxCountCount);
    if(ar.reading()) {
        mMaxCount.clear();
        for(int i = 0; i < maxCountCount; i++) {
            eResourceType rt; int c;
            ar.archiveField(("maxCount." + std::to_string(i)).c_str(),
                [&](eSaveArchive& it) {
                    it.field("resource", rt);
                    it.field("count", c);
                });
            mMaxCount[rt] = c;
        }
    } else {
        int i = 0;
        for(auto& kv : mMaxCount) {
            eResourceType rt = kv.first; int c = kv.second;
            ar.archiveField(("maxCount." + std::to_string(i++)).c_str(),
                [&](eSaveArchive& it) {
                    it.field("resource", rt);
                    it.field("count", c);
                });
        }
    }

    ar.payloadField("cart1",
        [this](eWriteStream& dst) { dst.writeCharacter(mCart1); },
        [this](eReadStream& src) {
            src.readCharacter(&getBoard(), [this](eCharacter* const c) {
                mCart1 = static_cast<eCartTransporter*>(c);
            });
        });
    ar.payloadField("cart2",
        [this](eWriteStream& dst) { dst.writeCharacter(mCart2); },
        [this](eReadStream& src) {
            src.readCharacter(&getBoard(), [this](eCharacter* const c) {
                mCart2 = static_cast<eCartTransporter*>(c);
            });
        });
}

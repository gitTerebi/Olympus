#include "eresourcebuildingbase.h"

#include "characters/actions/ecarttransporteraction.h"
#include "engine/e-game-board.h"
#include "enumbers.h"
#include "fileIO/esavearchive.h"
#include "fileIO/ejsonarchive.h"

#include <algorithm>

eResourceBuildingBase::eResourceBuildingBase(
        eGameBoard& board,
        const eBuildingType type,
        const int sw, const int sh,
        const int maxEmployees,
        const eResourceType resType,
        const eCityId cid) :
    eEmployingBuilding(board, type, sw, sh,
                       maxEmployees, cid),
    mResType(resType) {

}

eResourceBuildingBase::~eResourceBuildingBase() {
    if(mCart) mCart->kill();
}

void eResourceBuildingBase::timeChanged(const int by) {
    if(enabled()) {
        if(!mCart) {
            mCart = spawnCart(eCartActionTypeSupport::give);
            mCart->setMaxDistance(eNumbers::sResourceBuildingMaxResourceGiveDistance);
        } else {
            if(const auto action = dynamic_cast<eCartTransporterAction*>(mCart->action())) {
                if(!action->src()) action->setBuilding(this);
            }
            if(mCart->waiting() && mResource > 0) {
                const int a = mCart->add(mResType,
                                         std::min(mResource, maxCartLoad()));
                mResource -= a;
            }
        }
    }
    eEmployingBuilding::timeChanged(by);
}

int eResourceBuildingBase::maxCartLoad() const {
    return eResourceTypeHelpers::transportSize(mResType,
                                               getBoard().doubleCartCapacity());
}

int eResourceBuildingBase::add(const eResourceType type,
                               const int count) {
    if(type != mResType) return 0;

    const int r = std::clamp(mResource + count, 0, mMaxResource);
    const int result = r - mResource;
    mResource = r;
    return result;
}

int eResourceBuildingBase::take(const eResourceType type,
                                const int count) {
    if(!static_cast<bool>(type & mResType)) return 0;

    const int result = std::clamp(count, 0, mResource);
    mResource -= result;
    return result;
}

int eResourceBuildingBase::count(const eResourceType type) const {
    if(!static_cast<bool>(type & mResType)) return 0;
    return mResource;
}

int eResourceBuildingBase::spaceLeft(const eResourceType type) const {
    if(type != mResType) return 0;
    return mMaxResource - mResource;
}

std::vector<eCartTask> eResourceBuildingBase::cartTasks() const {
    std::vector<eCartTask> tasks;

    if(mResource > 0) {
        eCartTask task;
        task.fType = eCartActionType::give;
        task.fResource = mResType;
        task.fMaxCount = mResource;
        tasks.push_back(task);
    }

    return tasks;
}

void eResourceBuildingBase::read(eReadStream& src) {
    eEmployingBuilding::read(src);
    eSaveArchive ar(src);
    serialize(ar);
}

void eResourceBuildingBase::write(eWriteStream& dst) const {
    eEmployingBuilding::write(dst);
    eSaveArchive ar(dst);
    const_cast<eResourceBuildingBase*>(this)->serialize(ar);
}

void eResourceBuildingBase::serialize(eSaveArchive& ar) {
    ar.field("mResource", mResource);
    if(ar.reading()) {
        ar.readStream().readCharacter(&getBoard(), [this](eCharacter* const c) {
            mCart = static_cast<eCartTransporter*>(c);
        });
    } else {
        ar.writeStream().writeCharacter(mCart);
    }
}

void eResourceBuildingBase::serializeJson(eJsonArchive& ar) {
    eEmployingBuilding::serializeJson(ar);
    ar.field("mResource", mResource);
    if(ar.writing()) {
        eCharacter* raw = mCart;
        ar.characterRef("mCart", raw, getBoard());
    } else {
        ar.characterRef("mCart", [this](eCharacter* c) {
            mCart = static_cast<eCartTransporter*>(c);
        }, getBoard());
    }
}

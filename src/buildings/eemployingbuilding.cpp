#include "eemployingbuilding.h"

#include "engine/game-board.h"
#include "fileIO/save-archive.h"
#include "numbers.h"

eEmployingBuilding::eEmployingBuilding(
        GameBoard& board,
        const eBuildingType type,
        const int sw, const int sh,
        const int maxEmployees,
        const eCityId cid) :
    eBuildingWithResource(board, type, sw, sh, cid),
    mMaxEmployees(maxEmployees),
    mEmploymentWait(Numbers::sNewBuildingEmployWaitDays * Numbers::sDayLength) {
    board.registerEmplBuilding(this);
}

eEmployingBuilding::~eEmployingBuilding() {
    auto& board = ownerBoard();
    board.unregisterEmplBuilding(this);
}

void eEmployingBuilding::setEmployed(const int e) {
    if(mEmploymentWait > 0) return;
    mEmployed = e;
    setEnabled(mEmployed > 0);
}

void eEmployingBuilding::incTime(const int by) {
    eBuilding::incTime(by);
    if(mEmploymentWait <= 0) return;
    mEmploymentWait -= by;
    if(mEmploymentWait <= 0) {
        mEmploymentWait = 0;
        ownerBoard().distributeEmployees(cityId());
    }
}

double eEmployingBuilding::vacanciesFilledFraction() const {
    if(mMaxEmployees <= 0) return 1;
    return double(mEmployed)/mMaxEmployees;
}

double eEmployingBuilding::effectiveness() const {
    if(mShutDown) return 0.;
    const double ef = vacanciesFilledFraction();
    double blessed = 0.;
    if(eBuilding::blessed()) {
        blessed = 1.;
    } else if(eBuilding::cursed()) {
        blessed = -1.;
    }
    return ef*(1 + 0.5*blessed);
}

void eEmployingBuilding::setShutDown(const bool sd) {
    mShutDown = sd;
    if(sd) setEmployed(0);
}

void eEmployingBuilding::serializeFields(SaveArchive& ar) {
    eBuildingWithResource::serializeFields(ar);
    ar.field("shutDown", mShutDown);
    ar.field("maxEmployees", mMaxEmployees);
    ar.field("employed", mEmployed);
    ar.field("employmentWait", mEmploymentWait);
    const stdptr<eEmployingBuilding> tptr(this);
    ar.addPostFunc([tptr]() {
        if(!tptr) return;
        tptr->setEnabled(tptr->mEmploymentWait <= 0 && tptr->mEmployed > 0);
    }, "eEmployingBuilding::setEnabled");
}

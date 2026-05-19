#include "eemployingbuilding.h"

#include "engine/e-game-board.h"
#include "fileIO/esavearchive.h"

eEmployingBuilding::eEmployingBuilding(
        eGameBoard& board,
        const eBuildingType type,
        const int sw, const int sh,
        const int maxEmployees,
        const eCityId cid) :
    eBuildingWithResource(board, type, sw, sh, cid),
    mMaxEmployees(maxEmployees) {
    board.registerEmplBuilding(this);
}

eEmployingBuilding::~eEmployingBuilding() {
    auto& board = ownerBoard();
    board.unregisterEmplBuilding(this);
}

void eEmployingBuilding::setEmployed(const int e) {
    mEmployed = e;
    setEnabled(mEmployed > 0);
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

void eEmployingBuilding::serializeFields(eSaveArchive& ar) {
    eBuildingWithResource::serializeFields(ar);
    ar.field("shutDown", mShutDown);
    ar.field("maxEmployees", mMaxEmployees);
    ar.field("employed", mEmployed);
    const stdptr<eEmployingBuilding> tptr(this);
    ar.addPostFunc([tptr]() {
        if(!tptr) return;
        tptr->setEnabled(tptr->mEmployed > 0);
    }, "eEmployingBuilding::setEnabled");
}

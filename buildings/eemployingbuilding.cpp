#include "eemployingbuilding.h"

#include "engine/egameboard.h"

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
    auto& board = getBoard();
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
    const double ef = std::max(0.1, vacanciesFilledFraction());
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

void eEmployingBuilding::read(eReadStream& src) {
    eBuildingWithResource::read(src);
    src >> mShutDown;
    src >> mMaxEmployees;
    src >> mEmployed;
    setEnabled(mEmployed > 0);
}

void eEmployingBuilding::write(eWriteStream& dst) const {
    eBuildingWithResource::write(dst);
    dst << mShutDown;
    dst << mMaxEmployees;
    dst << mEmployed;
}

#include "ehousebase.h"

#include "engine/egameboard.h"

#include "characters/esettler.h"
#include "characters/ehomeless.h"
#include "characters/actions/emovetoaction.h"
#include "characters/actions/esettleraction.h"
#include "characters/actions/ekillcharacterfinishfail.h"
#include "enumbers.h"

eHouseBase::eHouseBase(eGameBoard& board,
                       const eBuildingType type,
                       const int sw, const int sh,
                       const std::vector<int>& maxPeople,
                       const eCityId cid) :
    eBuilding(board, type, sw, sh, cid),
    mMaxPeople(maxPeople) {
    auto& popData = eHouseBase::popData();
    popData.incVacancies(mMaxPeople[0]);
}

eHouseBase::~eHouseBase() {
    auto& board = getBoard();
    auto& popData = eHouseBase::popData();
    board.incPopulation(cityId(), -mPeople);
    popData.incVacancies(-vacancies());
}

void eHouseBase::timeChanged(const int by) {
    if(mPeople <= 0) return;
    mUpdateCulture += by;
    if(mUpdateCulture > eNumbers::sHouseCultureDecrementPeriod) {
        mUpdateCulture = 0;
        mPhilosophers = std::max(0, mPhilosophers - 1);
        mActors = std::max(0, mActors - 1);
        mAthletes = std::max(0, mAthletes - 1);
        mCompetitors = std::max(0, mCompetitors - 1);
    }
    eBuilding::timeChanged(by);
}

int eHouseBase::allCultureScience() const {
    int result = 0;
    if(mPhilosophers > 0) result++;
    if(mActors > 0) result++;
    if(mAthletes > 0) result++;
    if(mCompetitors > 0) result++;
    return result;
}

void eHouseBase::levelUp() {
    setLevel(mLevel + 1);
}

void eHouseBase::levelDown() {
    setLevel(mLevel - 1);
}

int eHouseBase::kill(int k) {
    k = std::clamp(k, 0, mPeople);
    setPeople(mPeople - k);
    return k;
}

int eHouseBase::moveIn(int c) {
    c = std::clamp(c, 0, vacancies());
    setPeople(mPeople + c);
    return c;
}

void eHouseBase::leave() {
    if(mPeople <= 0) return;
    const int leaveCount = std::min(4, mPeople);
    auto& board = getBoard();
    auto& popData = eHouseBase::popData();
    popData.incLeft(leaveCount);
    setPeople(mPeople - leaveCount);

    const auto c = e::make_shared<eSettler>(getBoard());
    c->setEmigrant(true);
    c->setBothCityIds(cityId());
    c->changeTile(centerTile());
    const stdptr<eSettler> cptr(c.get());
    const auto a = e::make_shared<eSettlerAction>(c.get());
    a->setNumberPeople(leaveCount);
    c->setAction(a);
    c->setActionType(eCharacterActionType::walk);
}

int eHouseBase::vacancies() const {
    return mMaxPeople[mLevel] - mPeople;
}

void eHouseBase::read(eReadStream& src) {
    eBuilding::read(src);

    src >> mPaidTaxes;
    src >> mPaidTaxesLastMonth;

    int level;
    src >> level;
    setLevel(level);
    int people;
    src >> people;
    setPeople(people);

    src >> mFood;
    src >> mFleece;
    src >> mOil;

    src >> mPhilosophers;
    src >> mActors;
    src >> mAthletes;
    src >> mCompetitors;

    src >> mUpdateCulture;

    // mPendingEvict and mEvictDelay not saved
}

void eHouseBase::write(eWriteStream& dst) const {
    eBuilding::write(dst);

    dst << mPaidTaxes;
    dst << mPaidTaxesLastMonth;

    dst << mLevel;
    dst << mPeople;

    dst << mFood;
    dst << mFleece;
    dst << mOil;

    dst << mPhilosophers;
    dst << mActors;
    dst << mAthletes;
    dst << mCompetitors;

    dst << mUpdateCulture;

    // mPendingEvict and mEvictDelay not saved
}

void eHouseBase::setLevel(const int l) {
    if(mLevel == l) return;

    const int ov = vacancies();
    const int maxLevel = mMaxPeople.size();
    mLevel = std::clamp(l, 0, maxLevel);
    const int nv = vacancies();

    auto& popData = eHouseBase::popData();
    popData.incVacancies(nv - ov);

    evict();
}

int eHouseBase::evict() {
    const int e = -vacancies();
    if(e <= 0) return 0;
    setPeople(mPeople - e);
    return e;
}

void eHouseBase::setPeople(const int p) {
    if(p == 0) setOnFire(false);
    if(p == mPeople) return;

    auto& board = getBoard();
    const auto cid = cityId();

    const int pc = p - mPeople;
    board.incPopulation(cid, pc);

    const int vb = vacancies();
    mPeople = p;
    const int va = vacancies();

    const auto popData = board.populationData(cid);
    if(popData) {
        const int vc = va - vb;
        popData->incVacancies(vc);
    }

    if(pc < 0) {
        mPendingEvict += -pc;
        mEvictDelay = 10;
    }
}

ePopulationData& eHouseBase::popData() {
    auto& board = getBoard();
    const auto cid = cityId();
    const auto c = board.boardCityWithId(cid);
    return c->populationData();
}

#include "small-house.h"

#include <algorithm>
#include "rand.h"
#include "textures/game-textures.h"
#include "engine/game-board.h"
#include "engine/difficulty.h"
#include "engine/model-data.h"
#include "engine/eplague.h"

#include "characters/esick.h"
#include "characters/edisgruntled.h"
#include "characters/actions/esickdisgruntledaction.h"

#include "characters/ehomeless.h"
#include "characters/actions/esettleraction.h"

#include "buildings/epalace.h"

#include "language.h"
#include "numbers.h"
#include "fileIO/save-archive.h"

static std::vector<int> sCommonCapacity(GameBoard& board, const eCityId cid) {
    const auto pid = board.cityIdToPlayerId(cid);
    const auto diff = board.difficulty(pid);
    std::vector<int> v(7, 0);
    for(int i = 0; i < 7; i++) {
        if(const auto r = ModelData::instance().houseReq(diff, i, false)) {
            v[i] = r->capacity;
        }
    }
    return v;
}

SmallHouse::SmallHouse(GameBoard &board, const eCityId cid) : eHouseBase(board, eBuildingType::commonHouse, 2, 2,
                                                                            sCommonCapacity(board, cid), cid)
{
    GameTextures::loadCommonHouse();
    if (atlantean())
    {
        GameTextures::loadPoseidonCommonHouse();
    }
}

SmallHouse::~SmallHouse()
{
    if (mSick)
        mSick->kill();
    if (mDisg)
        mDisg->kill();
    auto &board = ownerBoard();
    board.unregisterCommonHouse(this);
}

std::shared_ptr<Texture> SmallHouse::getTexture(const eTileSize size) const
{
    const int sizeId = static_cast<int>(size);
    const auto &blds = GameTextures::buildings();
    const auto &texs = blds[sizeId];

    if (mPeople == 0)
        return blds[sizeId].fHouseSpace;
    const TextureCollection *coll = nullptr;
    if (atlantean())
    {
        const int id = mLevel - 2;
        if (id < 0)
        {
            coll = &texs.fCommonHouse[mLevel];
        }
        else
        {
            coll = &texs.fPoseidonCommonHouse[id];
        }
    }
    else
    {
        coll = &texs.fCommonHouse[mLevel];
    }
    const int texId = seed() % coll->size();
    return coll->getTexture(texId);
}

int SmallHouse::provide(const eProvide p, const int n)
{
    if (mPeople <= 0)
        return 0;
    int max = 8;
    int *value = nullptr;
    switch (p)
    {
    case eProvide::water:
        value = &mWater;
        break;
    case eProvide::hygiene:
        value = &mHygiene;
        max = 100;
        break;
    case eProvide::food:
        value = &mFood;
        max = 2 * mPeople;
        break;
    case eProvide::fleece:
        value = &mFleece;
        break;
    case eProvide::oil:
        value = &mOil;
        break;

    case eProvide::philosopherInventor:
        value = &mPhilosophers;
        break;
    case eProvide::actorAstronomer:
        value = &mActors;
        break;
    case eProvide::gymnastScholar:
        value = &mAthletes;
        break;
    case eProvide::competitorCurator:
        value = &mCompetitors;
        break;

    case eProvide::taxes:
    {
        if (mPaidTaxes)
            return 0;
        auto &b = getBoard();
        const auto cid = cityId();
        const auto p = b.palace(cid);
        if (!p || p->cursed())
            return 0;
        const auto pid = playerId();
        const auto diff = b.difficulty(pid);
        const int taxMult = DifficultyHelpers::taxMultiplier(
            diff, type(), mLevel);
        const double tax = Numbers::sCommonHousingTaxMulitplier *
                           mPeople * taxMult * b.taxRateF(cid);
        const int iTax = std::round(tax);
        b.payTaxes(cid, iTax, mPeople);
        mPaidTaxes = iTax;
        return iTax;
    }

    case eProvide::satisfaction:
    {
        if (mSatisfactionProvidedThisMonth)
            return 0;
        mSatisfactionProvidedThisMonth = true;
        const int p = Numbers::sWatchmanSatisfactionProvide;
        mSatisfaction = std::clamp(mSatisfaction + p, 0, 100);
        return 0;
    }
    default:
        return eBuilding::provide(p, n);
    }
    int add = 0;
    if (value)
    {
        add = std::clamp(n, 0, max - *value);
        *value += add;
    }
    updateLevel(0);
    return add;
}

void SmallHouse::timeChanged(const int by)
{
    updateLevel(by);
    if (mPeople <= 0)
    {
        mHygiene = 100;
        if (mPlague)
        {
            auto &board = getBoard();
            board.healHouse(this);
        }
        setDisgruntled(false);
        mSatisfaction = 100;
        mFoodSatisfaction = 100;
        mWaterSatisfaction = 100;
        mWorkSatisfaction = 100;
        mTaxSatisfaction = 100;
        eHouseBase::timeChanged(by);
        return;
    }
    mUpdateCulture += by;
    if (mUpdateWater > Numbers::sHouseWaterDecrementPeriod)
    {
        mUpdateWater = 0;
        mWater = std::max(0, mWater - 1);
    }
    mUpdateHygiene += by;
    if (mUpdateHygiene > Numbers::sHouseHygieneDecrementPeriod)
    {
        mUpdateHygiene = 0;
        const int subWater = (100 - mWaterSatisfaction) / 20;
        const int subFood = (100 - mFoodSatisfaction) / 20;
        const int sub = 5 + subWater + subFood;
        mHygiene = std::max(0, mHygiene - sub);
    }

    auto &b = getBoard();
    if (mPlague)
    {
        if (mHygiene > 25)
        {
            const double pm = Numbers::sHouseHealPlaguePeriodMultiplier;
            const double pbm = Numbers::sHouseHealPlaguePeriodBaseMultiplier;
            const double pe = Numbers::sHouseHealPlaguePeriodExponent;
            const int m4 = pm * pow(pbm / mHygiene, pe);
            if (by)
            {
                const int healPeriod = m4 / by;
                if (healPeriod && Rand::rand() % healPeriod == 0)
                {
                    b.healHouse(this);
                }
            }
        }
    }
    else
    {
        const double pm = Numbers::sHousePlagueRiskPeriodMultiplier;
        const double pbi = Numbers::sHousePlagueRiskPeriodBaseIncrement;
        const double pe = Numbers::sHousePlagueRiskPeriodExponent;
        const int m4 = pm * pow(pbi + mHygiene, pe);
        const auto pid = playerId();
        const auto diff = b.difficulty(pid);
        const int plagueRisk = DifficultyHelpers::houseDiseaseRisk(diff, mLevel, false);
        if (plagueRisk > 0 && by)
        {
            const int plaguePeriod = m4 / (by * plagueRisk);
            if (plaguePeriod && Rand::rand() % plaguePeriod == 0)
            {
                const auto center = centerTile();
                const int tx = center->x();
                const int ty = center->y();
                int dist;
                const auto cid = cityId();
                b.nearestPlague(cid, tx, ty, dist);
                if (dist > 5)
                    b.startPlague(this);
            }
        }
    }

    const auto cid = cityId();
    const auto s = b.sanctuary(cid, GodType::dionysus);
    const bool dion = s && s->finished();
    if (mDisgruntled)
    {
        if (dion)
        {
            setDisgruntled(false);
        }
        else if (mSatisfaction > 30 && by > 0)
        {
            const int p = Numbers::sHouseDisgruntledRemovePeriod;
            if (Rand::rand() % (p / (mSatisfaction * by)) == 0)
            {
                setDisgruntled(false);
            }
        }
    }
    else if (!dion)
    {
        const double pm = Numbers::sHouseDisgruntledRiskPeriodMultiplier;
        const double pbi = Numbers::sHouseDisgruntledRiskPeriodBaseIncrement;
        const double pe = Numbers::sHouseDisgruntledRiskPeriodExponent;
        const int m4 = pm * pow(pbi + mSatisfaction, pe);
        const auto pid = playerId();
        const auto diff = b.difficulty(pid);
        const int crimeRisk = DifficultyHelpers::houseCrimeRisk(diff, mLevel, false);
        if (crimeRisk > 0 && by)
        {
            const int crimePeriod = m4 / (by * crimeRisk);
            if (crimePeriod && Rand::rand() % crimePeriod == 0)
            {
                setDisgruntled(true);
            }
        }
    }

    {
        const auto s = b.sanctuary(cid, GodType::aphrodite);
        const bool aphr = s && s->finished();
        if (!aphr)
        {
            const double pm = Numbers::sHouseLeaveRiskPeriodMultiplier;
            const double pbi = Numbers::sHouseLeaveRiskPeriodBaseIncrement;
            const double pe = Numbers::sHouseLeaveRiskPeriodExponent;
            const int m4 = pm * pow(pbi + mSatisfaction, pe);
            // Vanilla: leaving is driven by unmet needs/appeal, not crime.
            if (by)
            {
                const int leavePeriod = m4 / by;
                if (leavePeriod && Rand::rand() % leavePeriod == 0)
                {
                    leave();
                }
            }
        }
    }
    mUpdateSatisfaction += by;
    const int supdate = Numbers::sHouseSatisfactionUpdatePeriod;
    if (mUpdateSatisfaction > supdate)
    {
        mUpdateSatisfaction = 0;
        updateSatisfaction();
    }

    if (mDisgruntled && !mDisg)
    {
        mSpawnDisg += by;
        const int d = Numbers::sHouseDisgruntledSpawnPeriod;
        if (mSpawnDisg > d)
        {
            mSpawnDisg = 0;
            spawnDisgruntled();
        }
    }
    if (mPlague && !mSick)
    {
        mSpawnSick += by;
        const int s = Numbers::sHouseSickSpawnPeriod;
        if (mSpawnSick > s)
        {
            mSpawnSick = 0;
            spawnSick();
        }
    }

    eHouseBase::timeChanged(by);
}

void SmallHouse::nextMonth()
{
    mPaidTaxesLastMonth = mPaidTaxes;
    mPaidTaxes = 0;
    mSatisfactionProvidedThisMonth = false;
    const int cfood = round(mPeople * 0.25);
    const int cfleece = (mLevel > 2 && mPeople > 0) ? 2 : 0;
    const int coil = (mLevel > 4 && mPeople > 0) ? 2 : 0;
    mFood = std::max(0, mFood - cfood);
    mFleece = std::max(0, mFleece - cfleece);
    mOil = std::max(0, mOil - coil);
}

void SmallHouse::setPlague(const bool p)
{
    mPlague = p;
    mSpawnSick = Rand::rand() % 10000;
}

void SmallHouse::setDisgruntled(const bool d)
{
    mDisgruntled = d;
    mSpawnDisg = __INT_MAX__ / 2;
}

bool SmallHouse::lowFood() const
{
    if (!mFood)
        return true;
    const int cfood = round(mPeople * 0.25);
    return mFood < cfood;
}

eHouseMissing SmallHouse::missing() const
{
    const int next = mLevel + 1;
    const bool devolving = !canStayAtLevel(mLevel);

    // At max level missing() is only used to explain a devolve; check stay reqs.
    if (next > 6) {
        if (!devolving) return eHouseMissing::nothing;
        // fall through to report why canStayAtLevel failed
        const double appeal = eHouseBase::appeal();
        const int pts = culturePoints();
        const auto& board = getBoard();
        const auto pid = board.cityIdToPlayerId(cityId());
        const auto diff = board.difficulty(pid);
        const auto req = DifficultyHelpers::houseLevelReq(diff, false, mLevel);
        if (mFood <= 0) return eHouseMissing::food;
        if (mWater <= 0) return eHouseMissing::water;
        if (mFleece <= 0) return eHouseMissing::fleece;
        if (mOil <= 0) return eHouseMissing::oil;
        if (appeal < req.fAppD) return eHouseMissing::appeal;
        if (pts < req.fEnt) return eHouseMissing::venues;
        return eHouseMissing::nothing;
    }

    if (mFood <= 0) return eHouseMissing::food;
    if (next >= 2 && mWater <= 0) return eHouseMissing::water;
    if (next >= 3 && mFleece <= 0) return eHouseMissing::fleece;
    if (next >= 5 && mOil <= 0) return eHouseMissing::oil;

    const double appeal = eHouseBase::appeal();
    const int pts = culturePoints();
    const auto& board = getBoard();
    const auto pid = board.cityIdToPlayerId(cityId());
    const auto diff = board.difficulty(pid);
    // When devolving use stay threshold (fAppD); otherwise use evolve threshold (fAppE).
    const auto req = DifficultyHelpers::houseLevelReq(diff, false, mLevel);
    const double appReq = devolving ? req.fAppD : req.fAppE;
    if (appeal < appReq) return eHouseMissing::appeal;
    if (pts < req.fEnt) return eHouseMissing::venues;
    return eHouseMissing::nothing;
}

void SmallHouse::serializeFields(SaveArchive& ar)
{
    eHouseBase::serializeFields(ar);
    ar.field("satisfactionProvidedThisMonth", mSatisfactionProvidedThisMonth);
    ar.field("updateSatisfaction", mUpdateSatisfaction);
    ar.field("satisfaction", mSatisfaction);
    ar.field("foodSatisfaction", mFoodSatisfaction);
    ar.field("waterSatisfaction", mWaterSatisfaction);
    ar.field("workSatisfaction", mWorkSatisfaction);
    ar.field("taxSatisfaction", mTaxSatisfaction);

    ar.field("updateWater", mUpdateWater);
    ar.field("updateHygiene", mUpdateHygiene);

    ar.field("water", mWater);
    ar.field("hygiene", mHygiene);
    ar.field("plague", mPlague);
    ar.field("disgruntled", mDisgruntled);

    ar.field("spawnSick", mSpawnSick);
    ar.characterAsField("sick", &getBoard(), mSick);
    ar.field("spawnDisgruntled", mSpawnDisg);
    ar.characterAsField("disgruntled", &getBoard(), mDisg);
}

std::string SmallHouse::sName(const int level)
{
    return Language::zeusText(28, 2 + level);
}

bool SmallHouse::hasRequiredForLevel(const int level) const
{
    return hasRequiredForLevelImpl(level, true);
}

bool SmallHouse::canStayAtLevel(const int level) const
{
    return hasRequiredForLevelImpl(level, false);
}

bool SmallHouse::hasRequiredForLevelImpl(const int level, const bool evolve) const
{
    if (level <= 0) return true;
    if (level > 6) return false;
    const double appeal = eHouseBase::appeal();
    const int pts = culturePoints();
    const auto& board = getBoard();
    const auto pid = board.cityIdToPlayerId(cityId());
    const auto diff = board.difficulty(pid);
    // Evolve uses CURRENT level's row (b = appeal to leave for next).
    // Devolve uses CURRENT level's row (a = appeal to drop below).
    const int srcLvl = evolve ? std::max(0, level - 1) : level;
    const auto req = DifficultyHelpers::houseLevelReq(diff, false, srcLvl);
    const double appReq = evolve ? req.fAppE : req.fAppD;
    if (appeal < appReq) return false;
    if (pts < req.fEnt) return false;
    switch (level)
    {
    case 1:
        return mFood > 0;
    case 2:
        return mFood > 0 && mWater > 0;
    case 3:
    case 4:
        return mFood > 0 && mWater > 0 && mFleece > 0;
    case 5:
    case 6:
        return mFood > 0 && mWater > 0 && mFleece > 0 && mOil > 0;
    }
    return false;
}

void SmallHouse::updateLevel(const int by)
{
    // 3 in-game days below requirement before a single level drops.
    const int devolveTicks = 3 * Numbers::sDayLength;
    if (hasRequiredForLevel(mLevel + 1) && canStayAtLevel(mLevel + 1))
    {
        setLevel(mLevel + 1);
        mDevolveDelay = 0;
    }
    else if (!canStayAtLevel(mLevel))
    {
        if (mDevolveDelay < devolveTicks)
        {
            // Only real elapsed time advances devolve. provide() passes
            // by=0 so goods deliveries cannot race the counter.
            mDevolveDelay += by;
        }
        else
        {
            setLevel(mLevel - 1);
            // spawn homeless immediately
            if (mPendingEvict > 0)
            {
                const auto board = &getBoard();
                const auto cid = cityId();
                int waitTime = 0;
                while (mPendingEvict > 0)
                {
                    const int spawnCount = std::min(8, mPendingEvict);
                    eHomeless::spawn(*board, centerTile(), cid, spawnCount, waitTime);
                    mPendingEvict -= spawnCount;
                    waitTime += 10 + Rand::rand() % 25;
                }
            }

            mDevolveDelay = 0;
        }
    }
    else
    {
        mDevolveDelay = 0;
    }
}

void SmallHouse::updateSatisfaction()
{
    const int weight = 9;
    const int div = weight + 1;

    const int foodSat = mFood == 0 ? 0 : 100;
    mFoodSatisfaction = (weight * mFoodSatisfaction + foodSat) / div;

    const int waterSat = mWater == 0 ? 0 : 100;
    mWaterSatisfaction = (weight * mWaterSatisfaction + waterSat) / div;

    auto &board = getBoard();
    const auto cid = cityId();
    const auto empData = board.employmentData(cid);
    const double ef = empData ? empData->employedFraction() : 0.;
    const int workSat = 100 * std::pow(ef, 4);
    mWorkSatisfaction = (weight * mWorkSatisfaction + workSat) / div;

    const auto taxRate = board.taxRate(cid);
    const auto pid = playerId();
    const auto diff = board.difficulty(pid);
    const int ts = DifficultyHelpers::taxSentiment(diff, taxRate);
    const int taxSatIfPaid = std::round(100. * (ts + 7.) / 14.);
    const int taxSat = mPaidTaxesLastMonth ? taxSatIfPaid : 100;
    mTaxSatisfaction = (weight * mTaxSatisfaction + taxSat) / div;

    const int m1 = std::min(mFoodSatisfaction, mWaterSatisfaction);
    const int m2 = std::min(m1, mWorkSatisfaction);
    const int satMin = mTaxSatisfaction - 50;
    const int satMax = 50 + mTaxSatisfaction;
    const int sat = std::clamp(m2, satMin, satMax);
    mSatisfaction = (weight * mSatisfaction + sat) / div;
}

void SmallHouse::spawnCharacter(const stdsptr<eCharacter> &c)
{
    auto ts = surroundingRoad(false, true);
    eTile *tile = nullptr;
    if (ts.empty())
    {
        tile = centerTile();
    }
    else
    {
        tile = ts[Rand::rand() % ts.size()];
    }
    c->changeTile(tile);
    if (!tile)
        return;
    const auto a = e::make_shared<eSickDisgruntledAction>(c.get(), this);
    c->setAction(a);
}

void SmallHouse::spawnSick()
{
    if (mSick)
        mSick->kill();
    const auto c = e::make_shared<eSick>(getBoard());
    mSick = c.get();
    spawnCharacter(c);
    const auto tile = c->tile();
    if (!tile)
        return;
    const auto cid = cityId();
    c->setBothCityIds(cid);
}

void SmallHouse::spawnDisgruntled()
{
    if (mDisg)
        mDisg->kill();
    const auto c = e::make_shared<eDisgruntled>(getBoard());
    mDisg = c.get();
    const auto cid = cityId();
    mDisg->setOnCityId(cid);
    mDisg->setCityId(eCityId::neutralAggresive);
    spawnCharacter(c);
}

#include "elite-housing.h"
#include "fileIO/esavearchive.h"

#include "engine/game-board.h"
#include "engine/difficulty.h"
#include "engine/model-data.h"
#include "erand.h"

#include "textures/egametextures.h"
#include "elanguage.h"
#include "enumbers.h"
#include "buildings/epalace.h"

#include "characters/ehomeless.h"
#include "characters/actions/esettleraction.h"

static std::vector<int> sEliteCapacity(GameBoard& board, const eCityId cid) {
    const auto pid = board.cityIdToPlayerId(cid);
    const auto diff = board.difficulty(pid);
    std::vector<int> v(5, 0);
    for(int i = 0; i < 4; i++) {
        if(const auto r = ModelData::instance().houseReq(diff, i, true)) {
            v[i + 1] = r->capacity;
        }
    }
    v[0] = v[1];
    return v;
}

EliteHousing::EliteHousing(GameBoard& board,
                             const eCityId cid) :
    eHouseBase(board, eBuildingType::eliteHousing,
               4, 4, sEliteCapacity(board, cid), cid) {}

eTextureSpace EliteHousing::getTextureSpace(
        const int tx, const int ty,
        const eTileSize size) const {
    const SDL_Point p{tx, ty};
    const auto& r = tileRect();
    if(!SDL_PointInRect(&p, &r)) return {nullptr};
    auto& board = getBoard();
    const auto dir = board.direction();
    const auto& coll = getTextureCollection(size);
    const int rx = r.x;
    const int ry = r.y;
    const int dx = tx - rx;
    const int dy = ty - ry;
    if(dx < 2 && dy < 2) { // top
        if(dir == eWorldDirection::N) {
            const auto& tex = coll.getTexture(3);
            return {tex, true, {r.x, r.y, 2, 2}};
        } else if(dir == eWorldDirection::E) {
            const int id = seed() % 2;
            const auto& tex = coll.getTexture(id);
            return {tex, true, {r.x, r.y, 2, 2}};
        } else if(dir == eWorldDirection::S) {
            const auto& tex = coll.getTexture(2);
            return {tex, true, {r.x, r.y, 2, 2}};
        } else { // if(dir == eWorldDirection::W) {
            const auto& tex = coll.getTexture(4);
            return {tex, true, {r.x, r.y, 2, 2}};
        }
    } else if(dx < 2 && dy >= 2) { // left
        if(dir == eWorldDirection::N) {
            const int id = seed() % 2;
            const auto& tex = coll.getTexture(id);
            return {tex, false, {r.x, r.y + 2, 2, 2}};
        } else if(dir == eWorldDirection::E) {
            const auto& tex = coll.getTexture(2);
            return {tex, false, {r.x, r.y + 2, 2, 2}};
        } else if(dir == eWorldDirection::S) {
            const auto& tex = coll.getTexture(4);
            return {tex, false, {r.x, r.y + 2, 2, 2}};
        } else { // if(dir == eWorldDirection::W) {
            const auto& tex = coll.getTexture(3);
            return {tex, false, {r.x, r.y + 2, 2, 2}};
        }
    } else if(dx >= 2 && dy < 2) { // right
        if(dir == eWorldDirection::N) {
            const auto& tex = coll.getTexture(4);
            return {tex, false, {r.x + 2, r.y, 2, 2}};
        } else if(dir == eWorldDirection::E) {
            const auto& tex = coll.getTexture(3);
            return {tex, false, {r.x + 2, r.y, 2, 2}};
        } else if(dir == eWorldDirection::S) {
            const int id = seed() % 2;
            const auto& tex = coll.getTexture(id);
            return {tex, false, {r.x + 2, r.y, 2, 2}};
        } else { // if(dir == eWorldDirection::W) {
            const auto& tex = coll.getTexture(2);
            return {tex, false, {r.x + 2, r.y, 2, 2}};
        }
    } else { // bottom
        if(dir == eWorldDirection::N) {
            const auto& tex = coll.getTexture(2);
            return {tex, false, {r.x + 2, r.y + 2, 2, 2}};
        } else if(dir == eWorldDirection::E) {
            const auto& tex = coll.getTexture(4);
            return {tex, false, {r.x + 2, r.y + 2, 2, 2}};
        } else if(dir == eWorldDirection::S) {
            const auto& tex = coll.getTexture(3);
            return {tex, false, {r.x + 2, r.y + 2, 2, 2}};
        } else { // if(dir == eWorldDirection::W) {
            const int id = seed() % 2;
            const auto& tex = coll.getTexture(id);
            return {tex, false, {r.x + 2, r.y + 2, 2, 2}};
        }
    }
    return {};
}

std::vector<eOverlay> EliteHousing::getOverlays(const eTileSize size) const {
//    auto& board = getBoard();
//    if(board.atlantean()) return {};
//    return getHorseOverlays(size);
    (void)size;
    return {};
}

std::shared_ptr<eTexture>
EliteHousing::getLeftTexture(const eTileSize size) const {
    const auto& coll = getTextureCollection(size);
    const int id = seed() % 2;
    return coll.getTexture(id);
}

std::shared_ptr<eTexture>
EliteHousing::getBottomTexture(const eTileSize size) const {
    const auto& coll = getTextureCollection(size);
    return coll.getTexture(2);
}

std::shared_ptr<eTexture>
EliteHousing::getTopTexture(const eTileSize size) const {
    const auto& coll = getTextureCollection(size);
    return coll.getTexture(3);
}

std::shared_ptr<eTexture>
EliteHousing::getRightTexture(const eTileSize size) const {
    const auto& coll = getTextureCollection(size);
    return coll.getTexture(4);
}

std::vector<eOverlay>
EliteHousing::getHorseOverlays(const eTileSize size) const {
    if(mLevel < 3 || mHorses < 1 || mPeople <= 0) {
        return {};
    }
    const int sizeId = static_cast<int>(size);
    const auto& texs = eGameTextures::buildings()[sizeId];

    eGameTextures::loadEliteHouse();
    const auto& coll = texs.fEliteHouseHorses;
    eOverlay h;
    h.fX = -2.0;
    h.fY = -2.5;
    h.fTex = coll.getTexture(mHorses - 1);
    return {h};
}

int EliteHousing::provide(const eProvide p, const int n) {
    if(mPeople <= 0) return 0;
    int max = 8;
    int* value = nullptr;
    switch(p) {
    case eProvide::food:
        value = &mFood;
        max = 2*(16 + mPeople + mHorses);
        break;
    case eProvide::fleece:
        value = &mFleece;
        max = 16;
        break;
    case eProvide::oil:
        value = &mOil;
        max = 16;
        break;
    case eProvide::wine:
        value = &mWine;
        max = 16;
        break;
    case eProvide::arms:
        max = 4;
        value = &mArms;
        break;
    case eProvide::horses:
        max = 4;
        value = &mHorses;
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

    case eProvide::taxes: {
        if(mPaidTaxes) return 0;
        auto& b = getBoard();
        const auto cid = cityId();
        const auto p = b.palace(cid);
        if(!p || p->cursed()) return 0;
        const auto pid = playerId();
        const auto diff = b.difficulty(pid);
        const int taxMult = DifficultyHelpers::taxMultiplier(
                                diff, type(), mLevel);
        const double tax = eNumbers::sEliteHousingTaxMultiplier *
                           mPeople * taxMult * b.taxRateF(cid);
        const int iTax = std::round(tax);
        b.payTaxes(cid, iTax, mPeople);
        mPaidTaxes = iTax;
        return iTax;
    }
    default:
        return eBuilding::provide(p, n);
    }
    int add = 0;
    if(value) {
        add = std::clamp(n, 0, max - *value);
        *value += add;
    }
    updateLevel();
    return add;
}

void EliteHousing::timeChanged(const int by) {
    const int lupdate = 1000;
    if(mUpdateLevel > lupdate) {
        mUpdateLevel -= lupdate;
        updateLevel();
    }
    eHouseBase::timeChanged(by);
}

void EliteHousing::nextMonth() {
    mPaidTaxesLastMonth = mPaidTaxes;
    mPaidTaxes = 0;
    const int cfood = round((mPeople + mHorses)*0.25);
    const int cfleece = 2;
    const int coil = 2;
    const int cwine = mLevel > 2 ? 2 : 0; // manor+ drinks wine
    mFood = std::max(0, mFood - cfood);
    mFleece = std::max(0, mFleece - cfleece);
    mOil = std::max(0, mOil - coil);
    mWine = std::max(0, mWine - cwine);
}

bool EliteHousing::lowFood() const {
    if(!mFood) return true;
    const int cfood = round((mPeople + mHorses)*0.25);
    return mFood < cfood;
}

void EliteHousing::removeArmor() {
    mArms = std::max(0, mArms - 1);
    updateLevel();
}

void EliteHousing::removeHorse() {
    mHorses = std::max(0, mHorses - 1);
    updateLevel();
}

eHouseMissing EliteHousing::missing() const {
    const double appeal = eHouseBase::appeal();
    const int pts = culturePoints();
    int nVenues = 0;
    if(mPhilosophers > 0) nVenues++;
    if(mActors > 0) nVenues++;
    if(mAthletes > 0) nVenues++;
    if(mCompetitors > 0) nVenues++;

    const auto& board = getBoard();
    const auto pid = board.cityIdToPlayerId(cityId());
    const auto diff = board.difficulty(pid);

    // find the next level this house is trying to reach
    const int target = std::min(mLevel + 1, 3);

    // appeal threshold comes from the source row (same as canEvolveTo)
    DifficultyHelpers::eHouseLevelReq srcReq;
    if(target == 0) {
        srcReq = DifficultyHelpers::houseLevelReq(diff, false, 6); // Townhouse
    } else {
        srcReq = DifficultyHelpers::houseLevelReq(diff, true, target - 1);
    }
    const auto tgt = DifficultyHelpers::houseLevelReq(diff, true, target);
    const auto mr = ModelData::instance().houseReq(diff, target, true);

    if(mFood <= 0)   return eHouseMissing::food;
    if(mFleece <= 0) return eHouseMissing::fleece;
    if(mOil <= 0)    return eHouseMissing::oil;
    if(nVenues < 3)  return eHouseMissing::venues;
    if(appeal < srcReq.fAppE) return eHouseMissing::appeal;
    if(pts < tgt.fEnt) return eHouseMissing::venues;
    if(mr) {
        if(mr->armor  > 0 && mArms   < mr->armor)  return eHouseMissing::arms;
        if(mr->wine   > 0 && mWine   < mr->wine)   return eHouseMissing::wine;
        if(mr->horses > 0 && mHorses < mr->horses) return eHouseMissing::horse;
        if(target == 3 && nVenues < 4) return eHouseMissing::venues;
    }
    return eHouseMissing::nothing;
}

void EliteHousing::serializeFields(eSaveArchive& ar) {
    eHouseBase::serializeFields(ar);
    ar.field("updateLevel", mUpdateLevel);
    ar.field("wine", mWine);
    ar.field("arms", mArms);
    ar.field("horses", mHorses);
}

std::string EliteHousing::sName(const int level) {
    return eLanguage::zeusText(28, 10 + level);
}

const eTextureCollection& EliteHousing::getTextureCollection(
        const eTileSize size) const {
    const int sizeId = static_cast<int>(size);
    const auto& blds = eGameTextures::buildings()[sizeId];
    if(atlantean()) {
        eGameTextures::loadPoseidonEliteHouse();
        if(mPeople <= 0) return blds.fPoseidonEliteHouse[0];
        return blds.fPoseidonEliteHouse[mLevel + 1];
    } else {
        eGameTextures::loadEliteHouse();
        if(mPeople <= 0) return blds.fEliteHouse[0];
        return blds.fEliteHouse[mLevel + 1];
    }
}

void EliteHousing::updateLevel() {
    const double appeal = eHouseBase::appeal();
    const int pts = culturePoints();
    const auto& board = getBoard();
    const auto pid = board.cityIdToPlayerId(cityId());
    const auto diff = board.difficulty(pid);
    auto canEvolveTo = [&](const int lvl) {
        // lvl 0=residence, 1=mansion, 2=manor, 3=estate
        if(lvl < 0 || lvl > 3) return false;
        // Threshold lives on SOURCE row: prior elite row's b, or Townhouse.b for lvl 0.
        DifficultyHelpers::eHouseLevelReq req;
        if(lvl == 0) {
            req = DifficultyHelpers::houseLevelReq(diff, false, 6); // Townhouse
        } else {
            req = DifficultyHelpers::houseLevelReq(diff, true, lvl - 1);
        }
        if(appeal < req.fAppE) return false;
        // Culture / arms / etc gates use target row.
        const auto tgt = DifficultyHelpers::houseLevelReq(diff, true, lvl);
        if(pts < tgt.fEnt) return false;
        if(mFood <= 0 || mFleece <= 0 || mOil <= 0) return false;
        const auto mr = ModelData::instance().houseReq(diff, lvl, true);
        if(mr) {
            if(mr->armor  > 0 && mArms   < mr->armor)  return false;
            if(mr->wine   > 0 && mWine   < mr->wine)   return false;
            if(mr->horses > 0 && mHorses < mr->horses) return false;
        }
        return true;
    };
    int newLevel = -1;
    for(int l = 3; l >= 0; l--) {
        if(canEvolveTo(l)) { newLevel = l; break; }
    }
    if(newLevel < 0) {
        setLevel(0);
    } else {
        setLevel(newLevel);
    }
    // spawn homeless immediately
    if(mPendingEvict > 0) {
        const auto board = &getBoard();
        const auto cid = cityId();
        int waitTime = 0;
        while(mPendingEvict > 0) {
            const int spawnCount = std::min(20, mPendingEvict);
            eHomeless::spawn(*board, centerTile(), cid, spawnCount, waitTime);
            mPendingEvict -= spawnCount;
            waitTime += 10 + eRand::rand() % 25;
        }
    }
}

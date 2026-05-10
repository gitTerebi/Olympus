#include "e-worldcity.h"

#include "elanguage.h"
#include "egifthelpers.h"
#include "evectorhelpers.h"
#include "engine/e-game-board.h"
#include "engine/edifficulty.h"
#include "gameEvents/invasions/einvasionevent.h"
#include "fileIO/esavearchive.h"
#include "erand.h"

#include <iterator>

eWorldCity::eWorldCity(const eCityType type,
                       const eCityId id,
                       const std::string &name,
                       const double x, const double y) : mType(type), mCityId(id), mName(name), mX(x), mY(y) {}

std::string eWorldCity::sTypeName(const eCityType type)
{
    const int group = 39;
    int string = -1;
    switch (type)
    {
    case eCityType::parentCity:
        string = 0;
        break;
    case eCityType::colony:
        string = 1;
        break;
    case eCityType::foreignCity:
        string = 2;
        break;
    case eCityType::distantCity:
        string = 4;
        break;
    case eCityType::enchantedPlace:
        string = 5;
        break;
    case eCityType::destroyedCity:
        string = 6;
        break;
    }
    return eLanguage::zeusText(group, string);
}

bool eWorldCity::isDistant() const
{
    return mType == eCityType::distantCity;
}

bool eWorldCity::isVassal() const
{
    return mType == eCityType::foreignCity &&
           mRel == eForeignCityRelationship::vassal;
}

bool eWorldCity::isColony() const
{
    return mType == eCityType::colony;
}

bool eWorldCity::isParentCity() const
{
    return mType == eCityType::parentCity;
}

bool eWorldCity::isRival() const
{
    return mType == eCityType::foreignCity &&
           mRel == eForeignCityRelationship::rival;
}

bool eWorldCity::isAlly() const
{
    return mType == eCityType::foreignCity &&
           mRel == eForeignCityRelationship::ally;
}

bool eWorldCity::isOnBoardColony() const
{
    return mIsOnBoard && isColony();
}

bool eWorldCity::isOnBoardNeutral() const
{
    return mIsOnBoard && (mPlayerId == ePlayerId::neutralFriendly ||
                          mPlayerId == ePlayerId::neutralAggresive);
}

bool eWorldCity::isOnBoardEnemy() const
{
    return mIsOnBoard && mRel == eForeignCityRelationship::rival;
}

std::string eWorldCity::sDirectionName(const eDistantDirection d)
{
    switch (d)
    {
    case eDistantDirection::none:
        return "none";
    case eDistantDirection::N:
        return "N";
    case eDistantDirection::NE:
        return "NE";
    case eDistantDirection::E:
        return "E";
    case eDistantDirection::SE:
        return "SE";
    case eDistantDirection::S:
        return "S";
    case eDistantDirection::SW:
        return "SW";
    case eDistantDirection::W:
        return "W";
    case eDistantDirection::NW:
        return "NW";
    }
    return "none";
}

void eWorldCity::move(const double x, const double y)
{
    mX = x;
    mY = y;
}

std::string eWorldCity::sRelationshipName(const eForeignCityRelationship r)
{
    int group = 253;
    int string = -1;
    switch (r)
    {
    case eForeignCityRelationship::vassal:
        string = 2;
        break;
    case eForeignCityRelationship::ally:
        string = 0;
        break;
    case eForeignCityRelationship::rival:
        string = 1;
        break;
    }
    return eLanguage::zeusText(group, string);
}

std::string eWorldCity::sStateName(const eCityState s)
{
    int string = -1;
    switch (s)
    {
    case eCityState::active:
        string = 248;
        break;
    case eCityState::inactive:
        string = 249;
        break;
    }
    return eLanguage::zeusText(44, string);
}

std::string eWorldCity::sAttitudeName(const eCityAttitude at)
{

    const int group = 65;
    int string = -1;
    switch (at)
    {
    case eCityAttitude::philanthropic:
        string = 15;
        break;
    case eCityAttitude::resentful:
        string = 16;
        break;

    case eCityAttitude::helpful:
        string = 0;
        break;
    case eCityAttitude::congenial:
        string = 1;
        break;
    case eCityAttitude::sympathetic:
        string = 2;
        break;
    case eCityAttitude::apatheticA:
        string = 3;
        break;
    case eCityAttitude::annoyed:
        string = 4;
        break;

    case eCityAttitude::devoted:
        string = 10;
        break;
    case eCityAttitude::dedicated:
        string = 11;
        break;
    case eCityAttitude::loyal:
        string = 12;
        break;
    case eCityAttitude::bitter:
        string = 13;
        break;
    case eCityAttitude::angry:
        string = 14;
        break;

    case eCityAttitude::docile:
        string = 17;
        break;
    case eCityAttitude::hostile:
        string = 18;
        break;

    case eCityAttitude::admiring:
        string = 5;
        break;
    case eCityAttitude::respectful:
        string = 6;
        break;
    case eCityAttitude::apatheticR:
        string = 7;
        break;
    case eCityAttitude::displeased:
        string = 8;
        break;
    case eCityAttitude::furious:
        string = 9;
        break;
    default:
    case eCityAttitude::insubordinate:
        string = 19;
        break;
    }
    return eLanguage::zeusText(group, string);
}

eCityAttitude eWorldCity::attitudeClass(const ePlayerId pid) const
{
    eCityAttitude at;
    const int iat = attitude(pid);
    if (isAlly())
    {
        if (iat < 20)
            at = eCityAttitude::annoyed;
        else if (iat < 40)
            at = eCityAttitude::apatheticA;
        else if (iat < 60)
            at = eCityAttitude::sympathetic;
        else if (iat < 80)
            at = eCityAttitude::congenial;
        else
            at = eCityAttitude::helpful;
    }
    else if (isVassal() || isColony())
    {
        if (iat < 20)
            at = eCityAttitude::angry;
        else if (iat < 40)
            at = eCityAttitude::bitter;
        else if (iat < 60)
            at = eCityAttitude::loyal;
        else if (iat < 80)
            at = eCityAttitude::dedicated;
        else
            at = eCityAttitude::devoted;
    }
    else
    { // rival
        if (iat < 10)
            at = eCityAttitude::hostile;
        else if (iat < 20)
            at = eCityAttitude::furious;
        else if (iat < 40)
            at = eCityAttitude::displeased;
        else if (iat < 60)
            at = eCityAttitude::apatheticR;
        else if (iat < 80)
            at = eCityAttitude::respectful;
        else
            at = eCityAttitude::admiring;
    }
    return at;
}

double eWorldCity::attitude(const ePlayerId pid) const
{
    const auto it = mAtt.find(pid);
    if (it == mAtt.end())
        return 60;
    return it->second;
}

void eWorldCity::setAttitude(const double a, const ePlayerId pid)
{
    mAtt[pid] = std::clamp(a, 0., 100.);
}

void eWorldCity::incAttitude(const double a, const ePlayerId pid)
{
    setAttitude(attitude(pid) + a, pid);
}

std::vector<std::string> eWorldCity::sNames()
{
    std::vector<std::string> cityNames;
    for (int i = 0; i < 82; i++)
    {
        cityNames.push_back(eLanguage::zeusText(21, i));
    }
    return cityNames;
}

void eWorldCity::setName(const std::string &name)
{
    mName = name;
    const auto names = sNames();
    mNameString = eVectorHelpers::index(names, name);
}

std::string eWorldCity::nameWithId() const
{
    std::string result = mName;
    const int iid = static_cast<int>(mCityId);
    result += " [" + std::to_string(iid) + "]";
    return result;
}

std::vector<std::string> eWorldCity::sLeaders()
{
    std::vector<std::string> leaders;
    for (int i = 0; i < 84; i++)
    {
        leaders.push_back(eLanguage::zeusText(139, i));
    }
    return leaders;
}

void eWorldCity::setLeader(const std::string &name)
{
    mLeader = name;
    const auto names = sLeaders();
    mLeaderString = eVectorHelpers::index(names, name);
}

std::string eWorldCity::sNationalityName(const eNationality type)
{
    const int group = 37;
    int string = -1;
    switch (type)
    {
    case eNationality::greek:
        string = 0;
        break;
    case eNationality::trojan:
        string = 1;
        break;
    case eNationality::persian:
        string = 2;
        break;
    case eNationality::centaur:
        string = 3;
        break;
    case eNationality::amazon:
        string = 4;
        break;

    case eNationality::egyptian:
        string = 5;
        break;
    case eNationality::mayan:
        string = 6;
        break;
    case eNationality::phoenician:
        string = 7;
        break;
    case eNationality::oceanid:
        string = 8;
        break;
    case eNationality::atlantean:
        string = 9;
        break;
    default:
        break;
    }
    return eLanguage::zeusText(group, string);
}

std::string eWorldCity::anArmy() const
{
    const int group = 37;
    int string = -1;
    switch (mNationality)
    {
    case eNationality::greek:
        string = 0;
        break;
    case eNationality::trojan:
        string = 1;
        break;
    case eNationality::persian:
        string = 2;
        break;
    case eNationality::centaur:
        string = 3;
        break;
    case eNationality::amazon:
        string = 4;
        break;

    case eNationality::egyptian:
        string = 5;
        break;
    case eNationality::mayan:
        string = 6;
        break;
    case eNationality::phoenician:
        string = 7;
        break;
    case eNationality::oceanid:
        string = 8;
        break;
    case eNationality::atlantean:
        string = 9;
        break;
    default:
        break;
    }
    return eLanguage::zeusText(group, 22 + string);
}

void eWorldCity::nextMonth(eGameBoard *const board)
{
    const auto ppid = board->personPlayer();
    const auto attitude = attitudeClass(ppid);
    if (isRival() && active() && visible() &&
        attitude == eCityAttitude::hostile)
    {
        auto targetCid = board->personPlayerCapital();
        if (targetCid == eCityId::neutralFriendly ||
            targetCid == eCityId::neutralAggresive)
        {
            targetCid = board->world().currentCityId();
        }
        const auto targetCity = board->world().cityWithId(targetCid);
        const bool canInvade = targetCity &&
                               !board->hasActiveInvasions(targetCid) &&
                               board->date().year() > mNextInvasionYear;
        if (canInvade && (eRand::rand() % 12 == 0))
        {
            const auto e = e::make_shared<eInvasionEvent>(targetCid, eGameEventBranch::root, *board);
            const auto attacker = board->world().cityWithId(mCityId);

            // max banners in city 20
            const int unitCount =
                static_cast<int>(mMilitaryStrength * eDifficultyHelpers::costMultiplier(board->difficulty(attacker->playerId()))) * 8;

            e->setSingleCity(attacker);
            e->setMinPointId(1);
            e->setMaxPointId(16);
            e->setMinCount(static_cast<int>(unitCount / 2));
            e->setMaxCount(unitCount);
            e->setWarningMonths(1);
            auto date = board->date() + 31;
            e->initializeDate(date);
            board->addRootGameEvent(e);
            mNextInvasionYear = board->date().year() + 1 + eRand::rand() % 3;
        }
    }

    const auto diff = board->personPlayerDifficulty();
    double mult;
    switch (diff)
    {
    case eDifficulty::beginner:
        mult = 3;
        break;
    case eDifficulty::mortal:
        mult = 4;
        break;
    case eDifficulty::hero:
        mult = 4.5;
        break;
    case eDifficulty::titan:
        mult = 5;
        break;
    case eDifficulty::olympian:
    default:
        mult = 6;
        break;
    }
    const int targetTroops = std::round(mMilitaryStrength * mult * sqrt(mYearsElapsed + 1));
    if (mTroops < targetTroops)
    {
        mTroops++;
    }
}

void eWorldCity::nextYear()
{
    for (auto &b : mBuys)
    {
        b.zeroUsed();
    }
    for (auto &s : mSells)
    {
        s.zeroUsed();
    }
    mReceived.clear();
    mYearsElapsed++;
}

void eWorldCity::troopsByType(const int troops,
                              int &infantry,
                              int &cavalry,
                              int &archers) const
{
    const auto n = nationality();
    switch (n)
    {
    case eNationality::greek:
    {
        infantry = std::ceil(0.6 * troops);
        cavalry = std::ceil(0.2 * troops);
        archers = std::ceil(0.2 * troops);
    }
    break;
    case eNationality::trojan:
    {
        infantry = std::ceil(0.5 * troops);
        cavalry = std::ceil(0.3 * troops);
        archers = std::ceil(0.2 * troops);
    }
    break;
    case eNationality::persian:
    {
        infantry = std::ceil(0.3 * troops);
        cavalry = std::ceil(0.3 * troops);
        archers = std::ceil(0.4 * troops);
    }
    break;
    case eNationality::centaur:
    {
        infantry = 0;
        cavalry = std::ceil(0.5 * troops);
        archers = std::ceil(0.5 * troops);
    }
    break;
    case eNationality::amazon:
    {
        infantry = std::ceil(0.75 * troops);
        cavalry = 0;
        archers = std::ceil(0.25 * troops);
    }
    break;

    case eNationality::egyptian:
    {
        infantry = std::ceil(0.5 * troops);
        cavalry = std::ceil(0.2 * troops);
        archers = std::ceil(0.3 * troops);
    }
    break;
    case eNationality::mayan:
    {
        infantry = std::ceil(0.25 * troops);
        cavalry = 0;
        archers = std::ceil(0.75 * troops);
    }
    break;
    case eNationality::phoenician:
    {
        infantry = 0;
        cavalry = std::ceil(0.3 * troops);
        archers = std::ceil(0.7 * troops);
    }
    break;
    case eNationality::oceanid:
    {
        infantry = std::ceil(0.5 * troops);
        cavalry = 0;
        archers = std::ceil(0.5 * troops);
    }
    break;
    case eNationality::atlantean:
    {
        infantry = std::ceil(0.4 * troops);
        cavalry = std::ceil(0.3 * troops);
        archers = std::ceil(0.3 * troops);
    }
    break;
    }
}

void eWorldCity::troopsByType(int &infantry,
                              int &cavalry,
                              int &archers) const
{
    const int troops = std::max(12, 2 * this->troops() / 3);
    troopsByType(troops, infantry, cavalry, archers);
}

int eWorldCity::shields() const
{
    return std::clamp(1 + mTroops / 20, 1, 5);
}

void eWorldCity::setMilitaryStrength(const int s)
{
    mMilitaryStrength = std::clamp(s, 1, 6);
}

void eWorldCity::setWaterTrade(const bool w, const eCityId cid)
{
    if (w)
        mWaterTrade.insert(cid);
    else
        mWaterTrade.erase(cid);
}

bool eWorldCity::waterTrade(const eCityId cid) const
{
    const auto it = mWaterTrade.find(cid);
    return it != mWaterTrade.end();
}

bool eWorldCity::trades() const
{
    // if(mBuys.empty() && mSells.empty()) return false;
    if (rebellion())
        return false;
    if (isRival())
        return false;
    if (tradeShutdown())
        return false;
    if (mConqueredBy)
        return false;
    if (!visible())
        return false;
    if (!active())
        return false;
    return true;
}

bool eWorldCity::buys(const eResourceType type) const
{
    for (const auto &rt : mBuys)
    {
        if (static_cast<bool>(rt.fType & type))
            return true;
    }
    return false;
}

void eWorldCity::addBuys(const eResourceTrade &b)
{
    mBuys.push_back(b);
}

void eWorldCity::changeDemand(const eResourceType res, const int by)
{
    for (auto &b : mBuys)
    {
        if (b.fType != res)
            continue;
        b.fMax = std::clamp(b.fMax + by, 0, 100);
    }
}

void eWorldCity::changeSupply(const eResourceType res, const int by)
{
    for (auto &s : mSells)
    {
        if (s.fType != res)
            continue;
        s.fMax = std::clamp(s.fMax + by, 0, 100);
    }
}

bool eWorldCity::sells(const eResourceType type) const
{
    for (const auto &rt : mSells)
    {
        if (static_cast<bool>(rt.fType & type))
            return true;
    }
    return false;
}

void eWorldCity::addSells(const eResourceTrade &s)
{
    mSells.push_back(s);
}

void eWorldCity::write(eWriteStream &dst) const
{
    eSaveArchive ar(dst);
    const_cast<eWorldCity *>(this)->serialize(ar, nullptr);
}

void serializeResourceTrades(eSaveArchive &ar,
                             const char *const name,
                             std::vector<eResourceTrade> &v)
{
    int n = ar.writing() ? static_cast<int>(v.size()) : 0;
    ar.field(std::string(name) + ".count", n);
    if (ar.reading())
        v.clear();
    for (int i = 0; i < n; i++)
    {
        auto &vv = ar.reading() ? v.emplace_back() : v[i];
        vv.serialize(ar);
    }
}

void eWorldCity::read(eReadStream &src, eWorldBoard *const board)
{
    eSaveArchive ar(src);
    serialize(ar, board);
    if (mNameString > -1 && mNameString < 82)
    {
        mName = eLanguage::zeusText(21, mNameString);
    }
    if (mLeaderString > -1 && mLeaderString < 84)
    {
        mLeader = eLanguage::zeusText(139, mLeaderString);
    }
}

void eWorldCity::serialize(eSaveArchive &ar, eWorldBoard *board)
{
    ar.field("mIOID", mIOID);
    ar.field("mCityId", mCityId);
    if (ar.reading())
    {
        ar.readStream().readCity(board, [this](const stdsptr<eWorldCity> &c)
                                 { mConqueredBy = c; });
    }
    else
    {
        ar.writeStream().writeCity(mConqueredBy.get());
    }
    ar.field("mPlayerId", mPlayerId);
    ar.field("mCapitalOf", mCapitalOf);
    ar.field("mIsCurrentCity", mIsCurrentCity);
    ar.field("mIsOnBoard", mIsOnBoard);
    ar.field("mType", mType);
    ar.field("mNationality", mNationality);
    ar.field("mDirection", mDirection);
    ar.field("mState", mState);
    ar.field("mNamePlace", mNamePlace);
    ar.field("mName", mName);
    ar.field("mNameString", mNameString);
    ar.field("mLeader", mLeader);
    ar.field("mLeaderString", mLeaderString);
    ar.field("mX", mX);
    ar.field("mY", mY);
    ar.field("mTradeShutdown", mTradeShutdown);
    ar.field("mRebellion", mRebellion);
    ar.field("mRel", mRel);

    int nrec;
    if (ar.writing())
        nrec = mReceived.size();
    ar.field("nrec", nrec);
    if (ar.reading())
        mReceived.clear();
    for (int i = 0; i < nrec; i++)
    {
        eResourceType type;
        int count;
        if (ar.writing())
        {
            auto it = mReceived.begin();
            std::advance(it, i);
            type = it->first;
            count = it->second;
        }
        ar.field("type", type);
        ar.field("count", count);
        if (ar.reading())
            mReceived[type] = count;
    }

    int natt;
    if (ar.writing())
        natt = mAtt.size();
    ar.field("natt", natt);
    if (ar.reading())
        mAtt.clear();
    for (int i = 0; i < natt; i++)
    {
        ePlayerId pid;
        double att;
        if (ar.writing())
        {
            auto it = mAtt.begin();
            std::advance(it, i);
            pid = it->first;
            att = it->second;
        }
        ar.field("pid", pid);
        ar.field("att", att);
        if (ar.reading())
            mAtt[pid] = att;
    }

    ar.field("mAbroad", mAbroad);
    ar.field("mMilitaryStrength", mMilitaryStrength);
    ar.field("mTroops", mTroops);
    ar.field("mYearsElapsed", mYearsElapsed);
    ar.field("mWealth", mWealth);

    int nc;
    if (ar.writing())
        nc = mWaterTrade.size();
    ar.field("nc", nc);
    if (ar.reading())
        mWaterTrade.clear();
    for (int i = 0; i < nc; i++)
    {
        eCityId cid;
        if (ar.writing())
        {
            auto it = mWaterTrade.begin();
            std::advance(it, i);
            cid = *it;
        }
        ar.field("cid", cid);
        if (ar.reading())
            mWaterTrade.insert(cid);
    }

    ar.field("mVisible", mVisible);
    serializeResourceTrades(ar, "mBuys", mBuys);
    serializeResourceTrades(ar, "mSells", mSells);
    ar.field("receiveTributeType", mReceiveTributeType);
    ar.field("receiveTributeCount", mReceiveTributeCount);
    ar.field("payTributeType", mPayTributeType);
    ar.field("payTributeCount", mPayTributeCount);
    ar.field("mNextInvasionYear", mNextInvasionYear);
}

void eWorldCity::gifted(const eResourceType type, const int count)
{
    const auto comp = [type](const std::pair<eResourceType, int> &r)
    {
        return r.first == type;
    };
    const auto it = std::find_if(mReceived.begin(), mReceived.end(), comp);
    if (it == mReceived.end())
    {
        mReceived[type] = count;
    }
    else
    {
        mReceived[type] += count;
    }
}

bool eWorldCity::acceptsGift(const eResourceType type,
                             const int count) const
{
    if (type == eResourceType::drachmas)
        return true;
    (void)count;
    const auto comp = [type](const std::pair<eResourceType, int> &r)
    {
        return r.first == type;
    };
    const auto it = std::find_if(mReceived.begin(), mReceived.end(), comp);
    if (it == mReceived.end())
    {
        return true;
    }
    else
    {
        const int max = 3 * eGiftHelpers::giftCount(type);
        return it->second < max;
    }
}

void eWorldCity::setPlayerId(const ePlayerId pid)
{
    mPlayerId = pid;
}

void eWorldCity::setCapitalOf(const ePlayerId pid)
{
    mCapitalOf = pid;
}

bool eWorldCity::isCapitalOf(const ePlayerId pid) const
{
    return mCapitalOf == pid;
}

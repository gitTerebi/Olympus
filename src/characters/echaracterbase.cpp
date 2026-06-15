#include "echaracterbase.h"

#include "fileIO/save-archive.h"

#include "actions/edieaction.h"

#include "gods/god.h"
#include "heroes/ehero.h"
#include "monsters/emonster.h"

eCharacterBase::eCharacterBase(const eCharacterType type) :
    mType(type) {

}

void eCharacterBase::setAttack(const double a) {
    mAttack = a;
}

void eCharacterBase::setHP(const double hp) {
    mHP = hp;
}

void eCharacterBase::setSpeed(const double s) {
    const double floor = 10.0;
    mSpeed = s < floor ? floor : s;
}

bool eCharacterBase::fighting() const {
    return mActionType == eCharacterActionType::fight ||
            mActionType == eCharacterActionType::fight2;
}

void eCharacterBase::setBothCityIds(const eCityId cid) {
    setCityId(cid);
    setOnCityId(cid);
}

bool eCharacterBase::isSoldier() const {
    const auto t = type();
    const int min = static_cast<int>(eCharacterType::rockThrower);
    const int max = static_cast<int>(eCharacterType::greekHorseman);
    const int bi = static_cast<int>(t);
    if(bi >= min && bi <= max) return true;
    return false;
}

bool eCharacterBase::isFighter() const {
    const bool s = isSoldier();
    if(s) return true;
    const auto t = type();
    return t == eCharacterType::enemyBoat ||
           t == eCharacterType::trireme;
}

bool eCharacterBase::isBoat() const {
    const auto t = type();
    return t == eCharacterType::tradeBoat ||
           t == eCharacterType::enemyBoat ||
           t == eCharacterType::trireme ||
           t == eCharacterType::fishingBoat;
}

bool eCharacterBase::isGod() const {
    bool result;
    God::sCharacterToGodType(type(), &result);
    return result;
}

bool eCharacterBase::isHero() const {
    bool result;
    eHero::sCharacterToHeroType(type(), &result);
    return result;
}

bool eCharacterBase::isMonster() const {
    bool result;
    eMonster::sCharacterToMonsterType(type(), &result);
    return result;
}

bool eCharacterBase::isImmortal() const {
    return isGod() || isHero() || isMonster();
}

void eCharacterBase::setActionType(const eCharacterActionType t) {
    mActionType = t;
}

void eCharacterBase::serializeFields(SaveArchive& ar) {
    ar.field("actionType", mActionType);
    ar.field("cityId", mCityId);
    ar.field("onCityId", mOnCityId);
    ar.field("busy", mBusy);
    ar.field("hp", mHP);
    ar.field("attack", mAttack);
    ar.field("speed", mSpeed);
    if(!ar.writing() && mSpeed < 10.0) mSpeed = 10.0;
    ar.field("atlantean", mAtlantean);
    ar.field("armor", mArmor);
    ar.field("armorVsMissiles", mArmorVsMissiles);
    ar.field("missileAttack", mMissileAttack);
    ar.field("attackFreq", mAttackFreq);
    ar.field("missileFreq", mMissileFreq);
    ar.field("range", mRange);
}

void eCharacterBase::serialize(SaveArchive& ar) {
    serializeFields(ar);
}

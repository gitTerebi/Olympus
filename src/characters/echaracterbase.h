#ifndef ECHARACTERBASE_H
#define ECHARACTERBASE_H

#include "actions/echaracteractiontype.h"

#include "fileIO/estreams.h"

#include "engine/ecityid.h"

class eSaveArchive;

enum class eCharacterType {
    none,

    settler,
    homeless,

    boar,
    deer,
    wolf,

    shepherd,
    goatherd,

    sheep,
    goat,
    cattle1,
    cattle2,
    cattle3,
    bull,
    horse,
    chariot,

    grower,

    peddler,

    trailer,
    ox,
    marbleMiner,
    lumberjack,
    bronzeMiner,
    silverMiner,

    trader,
    donkey,

    artisan,

    hunter,
    butcher,

    gymnast,
    philosopher,
    actor,
    competitor,

    scholar,
    astronomer,
    inventor,
    curator,

    taxCollector,
    watchman,
    waterDistributor,
    fireFighter,

    disgruntled,
    sick,

    archer,

    rockThrower,
    hoplite,
    horseman,
    // put new soldiers below

    hoplitePoseidon,
    chariotPoseidon,
    archerPoseidon,

    amazon,
    aresWarrior,

    trojanHoplite,
    trojanSpearthrower,
    trojanHorseman,

    centaurHorseman,
    centaurArcher,

    persianHoplite,
    persianArcher,
    persianHorseman,

    oceanidHoplite,
    oceanidSpearthrower,

    egyptianHoplite,
    egyptianArcher,
    egyptianChariot,

    atlanteanHoplite,
    atlanteanArcher,
    atlanteanChariot,

    phoenicianHorseman,
    phoenicianArcher,

    mayanHoplite,
    mayanArcher,

    // put new soldiers above
    greekRockThrower,
    greekHoplite,
    greekHorseman,

    healer,

    cartTransporter,
    porter,

    urchinGatherer,
    fishingBoat,
    tradeBoat,

    aphrodite,
    apollo,
    ares,
    artemis,
    athena,
    atlas,
    demeter,
    dionysus,
    hades,
    hephaestus,
    hera,
    hermes,
    poseidon,
    zeus,

    calydonianBoar, // artemis, theseus
    cerberus, // hades, hercules
    chimera, // atlas, bellerophon
    cyclops, // zeus, odysseus
    dragon, // ares, jason
    echidna, //  -, bellerophon
    harpies, // -, atalanta
    hector, // aphrodite, achilles
    hydra, // athena, hercules
    kraken, // poseidon, perseus
    maenads, // dionysus, achilles
    medusa, // demeter, perseus
    minotaur, // hermes, theseus
    scylla, // apollo, odysseus
    sphinx, // hera, atalanta
    talos, // hephaestus, jason
    satyr,

    achilles, // hector, maenads
    atalanta, // harpies, sphinx
    bellerophon, // chimera, echidna
    hercules, // cerberus, hydra
    jason, // dragon, talos
    odysseus, // cyclops, scylla
    perseus, // medusa, kraken
    theseus, // calydonianBoar, minotaur

    eliteCitizen,
    trireme,
    enemyBoat,
    orichalcMiner
};

inline bool eIsWildAnimal(const eCharacterType t) {
    return t == eCharacterType::boar ||
           t == eCharacterType::deer ||
           t == eCharacterType::wolf;
}

class eCharacterBase {
public:
    eCharacterBase(const eCharacterType type);

    bool busy() const { return mBusy; }
    void setBusy(const bool b) { mBusy = b; }

    // Runtime-only count of attackers that have CLAIMED this character as their
    // melee target (Augustus-style targeted_by). Set at target-pick time, before
    // the attacker arrives, so several retaliators picking on the same tick don't
    // all collapse onto one enemy — once an enemy is claimed enough times the
    // next picker looks elsewhere. NOT serialized: loads start at 0 and units
    // re-claim on the next combat tick.
    int targetedByCount() const { return mTargetedByCount; }
    void incTargetedBy() { mTargetedByCount++; }
    void decTargetedBy() { if(mTargetedByCount > 0) mTargetedByCount--; }

    void setAttack(const double a);
    double attack() const { return mAttack; }
    void setHP(const double hp);
    double hp() const { return mHP; }

    void setArmor(const int a) { mArmor = a; }
    int armor() const { return mArmor; }
    void setArmorVsMissiles(const int a) { mArmorVsMissiles = a; }
    int armorVsMissiles() const { return mArmorVsMissiles; }
    void setMissileAttack(const int a) { mMissileAttack = a; }
    int missileAttack() const { return mMissileAttack; }
    void setAttackFreq(const int f) { mAttackFreq = f; }
    int attackFreq() const { return mAttackFreq; }
    void setMissileFreq(const int f) { mMissileFreq = f; }
    int missileFreq() const { return mMissileFreq; }
    void setRange(const int r) { mRange = r; }
    int range() const { return mRange; }
    bool dead() const { return mHP <= 0 ||
                        mActionType == eCharacterActionType::die; }
    bool fighting() const;

    double speed() const { return mSpeed; }
    void setSpeed(const double s);

    void setBothCityIds(const eCityId cid);

    eCityId cityId() const { return mCityId; }
    virtual void setCityId(const eCityId i) { mCityId = i; }

    bool atlantean() const { return mAtlantean; }
    void setAtlantean(const bool a) { mAtlantean = a; }

    eCityId onCityId() const { return mOnCityId; }
    void setOnCityId(const eCityId i) { mOnCityId = i; }

    eCharacterActionType actionType() const { return mActionType; }
    void setActionType(const eCharacterActionType t);

    void setType(eCharacterType type) { mType = type; }
    eCharacterType type() const { return mType; }

    bool isSoldier() const;
    bool isFighter() const;
    bool isBoat() const;
    bool isGod() const;
    bool isHero() const;
    bool isMonster() const;
    bool isImmortal() const;

    void serialize(eSaveArchive& ar);
protected:
    virtual void serializeFields(eSaveArchive& ar);
private:
    eCharacterType mType;
    eCharacterActionType mActionType{eCharacterActionType::none};

    eCityId mCityId{eCityId::neutralFriendly};
    eCityId mOnCityId{eCityId::neutralFriendly};

    bool mAtlantean = false;

    bool mBusy = false;
    int mTargetedByCount = 0; // runtime-only; see targetedByCount()

    double mHP = 100;
    double mAttack = 0;
    double mSpeed = 52.5;

    int mArmor = 0;
    int mArmorVsMissiles = 0;
    int mMissileAttack = 0;
    int mAttackFreq = 0;
    int mMissileFreq = 0;
    int mRange = 0;
};

#endif // ECHARACTERBASE_H

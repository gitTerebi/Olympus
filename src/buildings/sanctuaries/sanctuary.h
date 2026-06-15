#ifndef SANCTUARY_H
#define SANCTUARY_H

#include "../eemployingbuilding.h"
#include "../emonument.h"
#include "numbers.h"

#include "esanctuaryblueprint.h"
#include "characters/gods/god.h"
#include "engine/edate.h"

class BuildingTextures;
class SaveArchive;
class eMonster;

enum class eHelpDenialReason {
    tooSoon,
    noTarget,
    error
};

class eSanctuary : public eMonument {
public:
    eSanctuary(GameBoard& board,
               const eBuildingType type,
               const int sw, const int sh,
               const int maxEmployees,
               const eCityId cid);
    ~eSanctuary();

    void erase() override;

    void timeChanged(const int by) override;
    void nextMonth() override;

    void buildingProgressed() override;

    std::vector<eTile*> warriorTiles() const;
    void addWarriorTile(eTile* const t);
    void addSpecialTile(eTile* const t);

    bool askForAttack(const eCityId cid, eHelpDenialReason& reason);
    bool askForHelp(eHelpDenialReason& reason);

    God* god() const { return mGod; }

    GodType godType() const;

    God* spawnGod();
    void spawnPatrolingGod();
    void spawnDefenderMinion();

    void godComeback();
    bool godAbroad() const { return mGodAbroad; }

    double helpTimeFraction() const;
    double helpAttackTimeFraction() const;

    static stdsptr<eSanctuary> sCreate(
            const eBuildingType type,
            const int sw, const int sh,
            GameBoard& board, const eCityId cid);

    static void sLoadMonumentTextures(const GodType type);

    void setSpawnWait(const int w);

    bool sacrificing() const;
    bool priestOut() const;
    int sacrificeDaysLeft() const;

    bool prayerReady() const { return mHelpTimer >= Numbers::sGodHelpPeriod; }
    bool aresBuffReady() const { return mAresBuffReady; }
    void consumeAresBuff() { mAresBuffReady = false; }
    void sendAresAbroad();

    void registerElement(const stdsptr<eSanctBuilding>& e) override;
    void boostHelpTimer();
protected:
    void serializeFields(SaveArchive& ar) override;
private:
    stdptr<God> mGod;
    stdptr<eMonster> mMinion;
    bool mGodAbroad = false;
    int mSpawnWait = 0;
    int mMinionSpawnWait = 0;

    bool mAskedForHelp = false;
    int mCheckHelpNeeded = 0;
    int mHelpTimer = 0;
    bool mAresBuffReady = false;

    std::vector<eTile*> mWarriorTiles;
    std::vector<eTile*> mSpecialTiles;
};

#endif // SANCTUARY_H

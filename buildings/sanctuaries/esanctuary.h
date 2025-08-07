#ifndef ESANCTUARY_H
#define ESANCTUARY_H

#include "../eemployingbuilding.h"
#include "../emonument.h"

#include "esanctuaryblueprint.h"
#include "characters/gods/egod.h"
#include "engine/edate.h"

class eBuildingTextures;

enum class eHelpDenialReason {
    tooSoon,
    noTarget,
    error
};

class eSanctuary : public eMonument {
public:
    eSanctuary(eGameBoard& board,
               const eBuildingType type,
               const int sw, const int sh,
               const int maxEmployees,
               const eCityId cid);
    ~eSanctuary();

    void erase() override;

    void timeChanged(const int by) override;
    void nextMonth() override;

    void buildingProgressed() override;

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;

    std::vector<eTile*> warriorTiles() const;
    void addWarriorTile(eTile* const t);
    void addSpecialTile(eTile* const t);

    bool askForAttack(const eCityId cid, eHelpDenialReason& reason);
    bool askForHelp(eHelpDenialReason& reason);

    eGod* god() const { return mGod; }

    eGodType godType() const;

    eGod* spawnGod();
    void spawnPatrolingGod();

    void godComeback();
    bool godAbroad() const { return mGodAbroad; }

    double helpTimeFraction() const;
    double helpAttackTimeFraction() const;

    static stdsptr<eSanctuary> sCreate(
            const eBuildingType type,
            const int sw, const int sh,
            eGameBoard& board, const eCityId cid);

    static void sLoadMonumentTextures(const eGodType type);

    void setSpawnWait(const int w);

    bool sacrificing() const;
private:
    stdptr<eGod> mGod;
    bool mGodAbroad = false;
    int mSpawnWait = 0;

    bool mAskedForHelp = false;
    int mCheckHelpNeeded = 0;
    int mHelpTimer = 10000000;

    std::vector<eTile*> mWarriorTiles;
    std::vector<eTile*> mSpecialTiles;
};

#endif // ESANCTUARY_H

#ifndef EPLAYERRAIDEVENT_H
#define EPLAYERRAIDEVENT_H

#include "player-conquest-event-base.h"

class SaveArchive;

class PlayerRaidEvent : public PlayerConquestEventBase {
public:
    PlayerRaidEvent(const eCityId cid,
                    const eGameEventBranch branch,
                    GameBoard& board);

    void initialize(const eEnlistedForces& forces,
                    const stdsptr<WorldCity>& city,
                    const eResourceType resource);

    static int raidTargetStrength(const int enemyStr);

    void trigger() override;
    std::string longName() const override;

protected:
    void serializeFields(SaveArchive& ar) override;
private:
    eResourceType mResource = eResourceType::none;
};

#endif // EPLAYERRAIDEVENT_H

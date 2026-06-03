#ifndef EPLAYERRAIDEVENT_H
#define EPLAYERRAIDEVENT_H

#include "eplayerconquesteventbase.h"

class eSaveArchive;

class ePlayerRaidEvent : public ePlayerConquestEventBase {
public:
    ePlayerRaidEvent(const eCityId cid,
                     const eGameEventBranch branch,
                     GameBoard& board);

    void initialize(const eEnlistedForces& forces,
                    const stdsptr<WorldCity>& city,
                    const eResourceType resource);

    void trigger() override;
    std::string longName() const override;

protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    eResourceType mResource = eResourceType::none;
};

#endif // EPLAYERRAIDEVENT_H

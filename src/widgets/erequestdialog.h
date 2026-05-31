#ifndef EREQUESTDIALOG_H
#define EREQUESTDIALOG_H

#include "emodal.h"

#include "engine/e-worldcity.h"

class eRequestDialog : public eModal {
public:
    using eModal::eModal;

    using eRequestFunction = std::function<void(eCityId, eResourceType)>;
    using eRequestDefenceFunc = std::function<void(eCityId)>;
    void initialize(const stdsptr<eWorldCity>& c,
                    eWorldBoard& board,
                    const eRequestFunction& func,
                    const eRequestDefenceFunc& requestDefensiveAid,
                    const eAction& requestStrike,
                    const ePlayerId pid);
};

#endif // EREQUESTDIALOG_H

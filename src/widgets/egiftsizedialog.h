#ifndef EGIFTSIZEDIALOG_H
#define EGIFTSIZEDIALOG_H

#include "emodal.h"

#include "engine/e-worldcity.h"

class eGiftSizeDialog : public eModal {
public:
    using eModal::eModal;

    using eRequestFunction = std::function<void(eResourceType,
                                                const int,
                                                const eCityId)>;
    void initialize(const eResourceType type,
                    const stdsptr<eWorldCity>& c,
                    const eRequestFunction& func,
                    const GameBoard& board,
                    const eCityId cid);
private:
    stdsptr<eWorldCity> mCity;
};

#endif // EGIFTSIZEDIALOG_H

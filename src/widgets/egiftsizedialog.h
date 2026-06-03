#ifndef EGIFTSIZEDIALOG_H
#define EGIFTSIZEDIALOG_H

#include "emodal.h"

#include "engine/world-city.h"

class eGiftSizeDialog : public eModal {
public:
    using eModal::eModal;

    using eRequestFunction = std::function<void(eResourceType,
                                                const int,
                                                const eCityId)>;
    void initialize(const eResourceType type,
                    const stdsptr<WorldCity>& c,
                    const eRequestFunction& func,
                    const GameBoard& board,
                    const eCityId cid);
private:
    stdsptr<WorldCity> mCity;
};

#endif // EGIFTSIZEDIALOG_H

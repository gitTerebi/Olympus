#ifndef EGIFTDIALOG_H
#define EGIFTDIALOG_H

#include "emodal.h"

#include "engine/e-worldcity.h"

class eGiftDialog : public eModal {
public:
    using eModal::eModal;

    using eRequestFunction = std::function<void(eResourceType, eCityId)>;
    void initialize(const stdsptr<eWorldCity>& c,
                    const eRequestFunction& func,
                    const eGameBoard& board);
private:
    stdsptr<eWorldCity> mCity;
};

#endif // EGIFTDIALOG_H

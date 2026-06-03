#ifndef EGIFTDIALOG_H
#define EGIFTDIALOG_H

#include "emodal.h"

#include "engine/world-city.h"

class eGiftDialog : public eModal {
public:
    using eModal::eModal;

    using eRequestFunction = std::function<void(eResourceType, eCityId)>;
    void initialize(const stdsptr<WorldCity>& c,
                    const eRequestFunction& func,
                    const GameBoard& board);
private:
    stdsptr<WorldCity> mCity;
};

#endif // EGIFTDIALOG_H

#ifndef EENLISTFORCESDIALOG_H
#define EENLISTFORCESDIALOG_H

#include "emodal.h"

#include "characters/eenlistedforces.h"

#include <deque>

class eEnlistForcesDialog : public eModal {
public:
    using eModal::eModal;

    using eEnlistAction = std::function<void(const eEnlistedForces&, eResourceType)>;
    void initialize(const eEnlistedForces& enlistable,
                    const std::vector<eCityId>& cids,
                    const std::vector<std::string>& cnames,
                    const std::vector<eHeroType>& heroesAbroad,
                    const eEnlistAction& action,
                    const std::vector<eResourceType>& plunderResources = {});
    void setEnemyStr(const int s);
protected:
    void paintEvent(ePainter& p) override;
private:
    void updateTipPositions();

    int mEnemyStr = 0;
    eAction mSelectionChanged;
    int mFrame = 0;

    struct eTip {
        eWidget* fWid = nullptr;
        int fLastFrame = 0;
    };

    std::deque<eTip> mTips;

    eEnlistedForces mSelected;
    eResourceType mSelectedPlunder = eResourceType::none;
};

#endif // EENLISTFORCESDIALOG_H

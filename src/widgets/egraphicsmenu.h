#ifndef EGRAPHICSMENU_H
#define EGRAPHICSMENU_H

#include "emodal.h"
#include "esettings.h"

inline bool operator==(const eSettings& s0, const eSettings& s1) {
    if(s0.fRes != s1.fRes) return false;
    if(s0.fDisplayMode != s1.fDisplayMode) return false;
    if(s0.fKeyScrollSpeed != s1.fKeyScrollSpeed) return false;
    if(s0.fDisableEdgeScroll != s1.fDisableEdgeScroll) return false;
    return true;
}

class eGraphicsMenu : public eModal {
public:
    eGraphicsMenu(const eSettings& iniSettings,
                  eMainWindow* const window);

    using eApplyAction = std::function<void(const eSettings&)>;
    using eDisplayModeA = std::function<void(const eDisplayMode)>;
    void initialize(const eApplyAction& settingsA,
                    const eDisplayModeA& displayModeA);

protected:
    void close() override;

private:
    const eSettings mIniSettings;
    eSettings mSettings;
};

#endif // EGRAPHICSMENU_H

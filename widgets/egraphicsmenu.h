#ifndef EGRAPHICSMENU_H
#define EGRAPHICSMENU_H

#include "epopupwidget.h"
#include "esettings.h"

inline bool operator==(const eSettings& s0, const eSettings& s1) {
    if(s0.fRes != s1.fRes) return false;
    if(s0.fFullscreen != s1.fFullscreen) return false;
    if(s0.fKeyScrollSpeed != s1.fKeyScrollSpeed) return false;
    if(s0.fDisableEdgeScroll != s1.fDisableEdgeScroll) return false;
    return true;
}

class eGraphicsMenu : public ePopupWidget {
public:
    eGraphicsMenu(const eSettings& iniSettings,
                  eMainWindow* const window);

    using eApplyAction = std::function<void(const eSettings&)>;
    using eFullscreenA = std::function<void(const bool)>;
    void initialize(const eApplyAction& settingsA,
                    const eFullscreenA& fullscreenA);

protected:
    void closePopup() override;

private:
    const eSettings mIniSettings;
    eSettings mSettings;
};

#endif // EGRAPHICSMENU_H

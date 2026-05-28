#ifndef EOPTIONSMENU_H
#define EOPTIONSMENU_H

#include "emodal.h"
#include "esettings.h"
#include "escrollbar.h"
#include "engine/difficulty.h"

#include <functional>
#include <string>
#include <vector>

class eLabel;

class eOptionsMenu : public eModal {
public:
    using eSetInt = std::function<void(const int)>;
    using eClampInt = std::function<int(const int)>;
    using eSetBool = std::function<void(const bool)>;

    struct eSliderItem {
        std::string fLabel;
        int fMin;
        int fMax;
        int fValue;
        std::string fSuffix;
        eClampInt fClamp;
        eSetInt fSet;
    };

    struct eHotkeyItem {
        std::string fLabel;
        eHotkeyId fId;
        SDL_Scancode fValue;
        std::function<void(const eHotkeyId, const SDL_Scancode)> fSet;
    };

    struct eCheckboxItem {
        std::string fLabel;
        bool fValue;
        eSetBool fSet;
        std::string fTooltip;
    };

    struct eDifficultyItem {
        std::string fLabel;
        std::function<Difficulty()> fGet;
        std::function<void(const Difficulty)> fSet;
    };

    struct ePage {
        std::string fButtonLabel;
        std::string fTitle;
        std::vector<eSliderItem> fSliders;
        std::vector<eHotkeyItem> fHotkeys;
        std::vector<std::string> fLines;
        std::vector<eCheckboxItem> fCheckboxes;
        std::vector<eDifficultyItem> fDifficulties;
    };

    eOptionsMenu(const std::vector<ePage>& pages,
                 eMainWindow* const window);

    void initialize();

private:
    void showPage(const int id);
    void clearPage();

    std::vector<ePage> mPages;
    eScrollViewport* mPageViewport = nullptr;
    eWidget* mPage = nullptr;
    eLabel* mMainTitle = nullptr;
};

#endif // EOPTIONSMENU_H

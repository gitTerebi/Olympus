#ifndef EOPTIONSMENU_H
#define EOPTIONSMENU_H

#include "emodal.h"
#include "settings.h"
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
    using eReopenPage = std::function<void(const int)>;

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
        HotkeyId fId;
        SDL_Scancode fValue;
        std::function<void(const HotkeyId, const SDL_Scancode)> fSet;
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

    // A labelled dropdown: shows the current option; clicking pops a chooser.
    struct eChoiceItem {
        std::string fLabel;
        std::vector<std::string> fOptions;
        int fValue;
        eSetInt fSet;
        std::function<bool(const int)> fReloadsUiScale;
    };

    struct ePage {
        std::string fButtonLabel;
        std::string fTitle;
        std::vector<eSliderItem> fSliders;
        std::vector<eHotkeyItem> fHotkeys;
        std::vector<std::string> fLines;
        std::vector<eCheckboxItem> fCheckboxes;
        std::vector<eDifficultyItem> fDifficulties;
        std::vector<eChoiceItem> fChoices;
    };

    eOptionsMenu(const std::vector<ePage>& pages,
                 MainWindow* const window,
                 const eReopenPage& reopenPage = nullptr);

    void initialize(const int initialPage = 0);

private:
    void showPage(const int id);
    void clearPage();
    void rebuild();

    std::vector<ePage> mPages;
    eReopenPage mReopenPage;
    eScrollViewport* mPageViewport = nullptr;
    eWidget* mPage = nullptr;
    eLabel* mMainTitle = nullptr;
    int mCurrentPage = 0;
};

#endif // EOPTIONSMENU_H

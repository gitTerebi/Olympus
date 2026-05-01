#ifndef EOPTIONSMENU_H
#define EOPTIONSMENU_H

#include "eframedwidget.h"
#include "esettings.h"

#include <functional>
#include <string>
#include <vector>

class eOptionsMenu : public eFramedWidget {
public:
    using eSetInt = std::function<void(const int)>;
    using eClampInt = std::function<int(const int)>;

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

    struct ePage {
        std::string fButtonLabel;
        std::string fTitle;
        std::vector<eSliderItem> fSliders;
        std::vector<eHotkeyItem> fHotkeys;
        std::vector<std::string> fLines;
    };

    eOptionsMenu(const std::vector<ePage>& pages,
                 eMainWindow* const window);

    void initialize();

private:
    void showPage(const int id);
    void clearPage();

    std::vector<ePage> mPages;
    class eOptionsPageViewport* mPageViewport = nullptr;
    eWidget* mPage = nullptr;
};

#endif // EOPTIONSMENU_H

#ifndef EOPTIONSMENU_H
#define EOPTIONSMENU_H

#include "eframedwidget.h"

class eOptionsMenu : public eFramedWidget {
public:
    using eGetInt = std::function<int()>;
    using eSetInt = std::function<void(const int)>;

    eOptionsMenu(const eGetInt& getKeyScrollSpeed,
                 const eSetInt& setKeyScrollSpeed,
                 eMainWindow* const window);

    void initialize();

private:
    void showDisplay();
    void showHotkeys();
    void clearPage();

    eGetInt mGetKeyScrollSpeed;
    eSetInt mSetKeyScrollSpeed;
    eWidget* mPage = nullptr;
};

#endif // EOPTIONSMENU_H

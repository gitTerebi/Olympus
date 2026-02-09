#ifndef ECHARACTERINFOWIDGET_H
#define ECHARACTERINFOWIDGET_H

#include "einfowidget.h"

class eCharacter;

class eCharacterInfoWidget : public eInfoWidget {
public:
    eCharacterInfoWidget(eMainWindow* const window,
                         eMainWidget* const mw);

    void initialize(const std::vector<eCharacter*> chars);
private:
    void setCharacter(eCharacter* const c);

    eLabel* mPortraitLabel = nullptr;
    eWidget* mTextWidget = nullptr;
    eLabel* mNameLabel = nullptr;
    eLabel* mOccupationLabel = nullptr;
    eLabel* mMsgLabel = nullptr;
};

#endif // ECHARACTERINFOWIDGET_H

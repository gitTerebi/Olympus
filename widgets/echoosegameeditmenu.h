#ifndef ECHOOSEGAMEEDITMENU_H
#define ECHOOSEGAMEEDITMENU_H

#include "emainmenubase.h"

#include "engine/ecampaign.h"

class eBitmapWidget;

class eChooseGameEditMenu : public eMainMenuBase {
public:
    using eMainMenuBase::eMainMenuBase;
    void initialize(const bool editor);

    void setGlossary(const eCampaignGlossary& g);

    bool keyPressEvent(const eKeyPressEvent& e) override;
    bool mouseReleaseEvent(const eMouseEvent& e) override;
private:
    eCampaignGlossary mSelected;
    eBitmapWidget* mBitmap = nullptr;
    eLabel* mTitle = nullptr;
    eLabel* mDesc = nullptr;
};

#endif // ECHOOSEGAMEEDITMENU_H

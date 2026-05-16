#ifndef ECHOOSEGAMEEDITMENU_H
#define ECHOOSEGAMEEDITMENU_H

#include "emainmenubase.h"

#include "engine/ecampaign.h"

class eBitmapWidget;
class eButtonBase;
class eScrollViewport;

class eChooseGameEditMenu : public eMainMenuBase {
public:
    using eMainMenuBase::eMainMenuBase;
    void initialize(const bool editor);

    void setGlossary(const eCampaignGlossary& g);

    bool keyPressEvent(const eKeyPressEvent& e) override;
    bool mouseReleaseEvent(const eMouseEvent& e) override;
private:
    void updateEpisodeList();

    eCampaignGlossary mSelected;
    int mSelectedEpisode = 0;
    eScrollViewport* mEpisodeViewport = nullptr;
    eButtonBase* mSelectedEpisodeButton = nullptr;
    eBitmapWidget* mBitmap = nullptr;
    eLabel* mTitle = nullptr;
    eLabel* mDesc = nullptr;
};

#endif // ECHOOSEGAMEEDITMENU_H

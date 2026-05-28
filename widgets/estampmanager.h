#ifndef ESTAMPMANAGER_H
#define ESTAMPMANAGER_H

#include "emodal.h"
#include "buildings/ebuilding.h"
#include "engine/stamps/estamptool.h"
#include "engine/difficulty.h"

#include <string>
#include <vector>

class eButtonBase;
class eLabel;
class eWidget;
class eScrollViewport;

bool eReadStampBlueprint(const std::string& path, std::vector<eStampElement>& elements);

class eStampManager : public eModal {
public:
    using eModal::eModal;

    void initialize(eStampTool* stampTool, Difficulty difficulty);
    void setTemplateSelectedAction(const eAction& action) {
        mTemplateSelectedAction = action;
    }
    void setCreateTemplateAction(const eAction& action) {
        mCreateTemplateAction = action;
    }
private:
    struct eTemplateButton {
        std::string fName;
        std::string fPath;
        int fPop = 0;
        int fCost = 0;
        eButtonBase* fButton = nullptr;
        eLabel* fNameLabel = nullptr;
    };

    void rebuildList();
    void selectTemplate(const std::string& name, const std::string& path);
    void deleteSelectedTemplate();
    void updateButtonColors();

    eStampTool* mStampTool = nullptr;
    Difficulty mDifficulty = Difficulty::mortal;
    eAction mTemplateSelectedAction;
    eAction mCreateTemplateAction;
    eScrollViewport* mViewport = nullptr;
    eWidget* mFilesWidget = nullptr;
    int mListWidth = 0;
    std::vector<eTemplateButton> mButtons;
};

#endif // ESTAMPMANAGER_H

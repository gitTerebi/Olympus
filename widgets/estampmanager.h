#ifndef ESTAMPMANAGER_H
#define ESTAMPMANAGER_H

#include "emodal.h"

#include <string>
#include <vector>

class eButtonBase;
class eStampTool;
class eWidget;
class eScrollViewport;

struct eStampElement {
    eBuildingType type;
};

bool eReadStampBlueprint(const std::string& path, std::vector<eStampElement>& elements);

class eStampManager : public eModal {
public:
    using eModal::eModal;

    void initialize(eStampTool* stampTool);
    void setTemplateSelectedAction(const eAction& action) {
        mTemplateSelectedAction = action;
    }
private:
    struct eTemplateButton {
        std::string fName;
        std::string fPath;
        int fPop = 0;
        eButtonBase* fButton = nullptr;
    };

    void rebuildList();
    void selectTemplate(const std::string& name, const std::string& path);
    void updateButtonColors();

    eStampTool* mStampTool = nullptr;
    eAction mTemplateSelectedAction;
    eScrollViewport* mViewport = nullptr;
    eWidget* mFilesWidget = nullptr;
    int mListWidth = 0;
    std::vector<eTemplateButton> mButtons;
};

#endif // ESTAMPMANAGER_H
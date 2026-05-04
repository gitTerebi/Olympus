#ifndef ELOADGAME_H
#define ELOADGAME_H

#include "epopupwidget.h"
#include "escrollbar.h"

class eLabel;
class eAcceptButton;
class eCancelButton;
class eLineEdit;

class eLoadGame : public ePopupWidget {
public:
    using ePopupWidget::ePopupWidget;

    using eFileFunc = std::function<bool(const std::string&)>;
    void intialize(const std::string& title,
                   const std::string& folder,
                   const eFileFunc& func,
                   const eAction& closeAction);

    void setFileName(const std::string& path);
    std::string filePath() const;
    void rebuildFileList();
protected:
    void closePopup() override;
private:

    eLabel* mTitleLabel = nullptr;
    eAcceptButton* mOk = nullptr;
    eCancelButton* mCancel = nullptr;
    eLineEdit* mLineEdit = nullptr;

    eScrollViewport* mViewport = nullptr;
    int mSwWidth = 0;
    eWidget* mFilesWidget = nullptr;

    std::string mFolder;
    eAction mCloseAction;
};

#endif // ELOADGAME_H

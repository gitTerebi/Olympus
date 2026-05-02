#ifndef EFILEWIDGET_H
#define EFILEWIDGET_H

#include "eframedwidget.h"

class eLabel;
class eAcceptButton;
class eCancelButton;
class eLineEdit;
class eScrollWidgetComplete;

class eFileWidget : public eFramedWidget {
public:
    using eFramedWidget::eFramedWidget;

    using eFileFunc = std::function<bool(const std::string&)>;
    void intialize(const std::string& title,
                   const std::string& folder,
                   const eFileFunc& func,
                   const eAction& closeAction);

    void setFileName(const std::string& path);
    std::string filePath() const;
    void rebuildFileList();
private:
    bool keyPressEvent(const eKeyPressEvent& e) override;

    eLabel* mTitleLabel = nullptr;
    eAcceptButton* mOk = nullptr;
    eCancelButton* mCancel = nullptr;
    eLineEdit* mLineEdit = nullptr;

    eScrollWidgetComplete* mScrollCont = nullptr;
    eWidget* mFilesWidget = nullptr;

    std::string mFolder;
    eAction mCloseAction;
};

#endif // EFILEWIDGET_H

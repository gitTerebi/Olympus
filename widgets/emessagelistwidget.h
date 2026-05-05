#ifndef EMESSAGELISTWIDGET_H
#define EMESSAGELISTWIDGET_H

#include <functional>

#include "eclosabledialog.h"
#include "elabel.h"
#include "emessage.h"
#include "engine/eeventdata.h"

class eScrollViewport;
class eScrollBar;
class eWidget;

class eMessageListWidget : public eClosableDialog {
public:
    using eClosableDialog::eClosableDialog;

    using eOpenMessage = std::function<void(eEventData, eMessage)>;
    void initialize(const eOpenMessage& openMsg, const eAction& closeAction = nullptr);

    void addMessage(const eEventData& ed, const eMessage& msg, const eDate& date);

    int unreadCount() const { return mUnreadCount; }
    void markAllRead();

    using eUnreadChangedAction = std::function<void(int)>;
    void setUnreadChangedAction(const eUnreadChangedAction& a) { mUnreadChanged = a; }
protected:
    bool keyPressEvent(const eKeyPressEvent& e) override;
    bool mouseReleaseEvent(const eMouseEvent& e) override;
private:
    void rebuildList();
    void notifyUnread();

    struct eLoggedMessage {
        eEventData fEd;
        eMessage fMsg;
        bool fRead = false;
        eDate fDate;
        std::string fFormattedTitle;
        std::string fDateStr;
    };

    eUnreadChangedAction mUnreadChanged;
    eOpenMessage mOpenMsg;
    class eMessageListRow : public eWidget {
    public:
        eMessageListRow(eMainWindow* window, eLabel* date, eLabel* title, std::function<void()> onClick)
            : eWidget(window), mDate(date), mTitle(title), mOnClick(onClick) {
            setNoPadding();
            addWidget(date);
            addWidget(title);
        }
    protected:

        bool mousePressEvent(const eMouseEvent&) override {
            mOnClick();
            return true;
        }
    private:
        eLabel* mDate;
        eLabel* mTitle;
        std::function<void()> mOnClick;
    };

    eAction mOnClose;
    eScrollViewport* mViewport = nullptr;
    eScrollBar* mSidebar = nullptr;
    eWidget* mContentArea = nullptr;
    int mVpWidth = 0;
    int mTp = 0;
    std::vector<eLoggedMessage> mMessages;
    int mUnreadCount = 0;
};

#endif // EMESSAGELISTWIDGET_H

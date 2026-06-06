#ifndef E_MESSAGE_LIST_WIDGET_H
#define E_MESSAGE_LIST_WIDGET_H

#include <functional>

#include "emodal.h"
#include "elabel.h"
#include "emessage.h"
#include "engine/eeventdata.h"

class eScrollViewport;
class eScrollBar;
class eWidget;
class GameBoard;

class eMessageListWidget : public eModal {
public:
    using eModal::eModal;

    using eOpenMessage = std::function<void(eEventData, eMessage)>;
    void initialize(const eOpenMessage& openMsg, const eAction& closeAction = nullptr);

    void show();
    void addMessage(const eEventData& ed, const eMessage& msg, const eDate& date);
    void addSavedMessage(const eEventData& ed, const eMessage& msg,
                         const eDate& date, const bool read);
    void setBoard(GameBoard* board);
    using eReadChangedAction = std::function<void(int)>;
    void setReadChangedAction(const eReadChangedAction& a) { mReadChanged = a; }

    int unreadCount() const { return mUnreadCount; }
    void markAllRead();

    using eUnreadChangedAction = std::function<void(int)>;
    void setUnreadChangedAction(const eUnreadChangedAction& a) { mUnreadChanged = a; }
protected:
    void close() override { hide(); if(mOnClose) mOnClose(); }
    bool keyPressEvent(const eKeyPressEvent& e) override;
private:
    void rebuildList();
    void requestRebuildList();
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
    eReadChangedAction mReadChanged;
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
    GameBoard* mBoard = nullptr;
    eScrollViewport* mViewport = nullptr;
    eScrollBar* mSidebar = nullptr;
    eWidget* mContentArea = nullptr;
    int mVpWidth = 0;
    int mTp = 0;
    std::vector<eLoggedMessage> mMessages;
    int mUnreadCount = 0;
    bool mListDirty = false;
};

#endif // E_MESSAGE_LIST_WIDGET_H

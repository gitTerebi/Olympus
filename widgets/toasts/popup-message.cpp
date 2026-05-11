#include "widgets/egamewidget.h"
#include "emainwindow.h"

#include "widgets/e-message-list-widget.h"
#include "widgets/e-message-box.h"
#include "engine/e-game-board.h"
#include "elanguage.h"
#include "estringhelpers.h"

void formatStoredMessage(eMessage& msg, const eEventData& ed, const std::string& playerName);

void eGameWidget::showMessage(eEventData &ed,
                              const eMessageType &msg,
                              const bool prepend)
{
    showMessage(ed, msg.fFull, prepend, false, true);
}

void eGameWidget::showMessage(eEventData &ed,
                              const eEventMessageType &msg,
                              const bool prepend)
{
    eMessageType m = msg;
    std::string reason = ed.fReason;
    if (reason.empty())
        reason = msg.fNoReason;
    eStringHelpers::replace(m.fFull.fText, "[reason_phrase]", reason);
    showMessage(ed, m.fFull, prepend, false, true);
}

void eGameWidget::showMessage(eEventData &ed,
                              const eMessage &msg,
                              const bool prepend,
                              const bool forcePopup,
                              const bool logToHistory)
{
    if (logToHistory && mMsgListWidget) {
        auto storedMsg = msg;
        formatStoredMessage(storedMsg, ed, window()->leader());
        const eDate messageDate = mBoard->date();
        mMsgListWidget->addMessage(ed, storedMsg, messageDate);
        mBoard->addMessageLog(ed, storedMsg, messageDate);
    }

    const auto &target = ed.fTarget;
    const auto ppid = mBoard->personPlayer();
    if (target.isPlayerTarget())
    {
        const auto pid = target.playerTarget();
        if (pid != ppid)
            return;
    }
    else if (target.isCityTarget())
    {
        const auto cid = target.cityTarget();
        const auto pid = mBoard->cityIdToPlayerId(cid);
        if (pid != ppid)
            return;
    }

    const bool requiresAction = ed.fCloseOnAction || !ed.fCityConditionalActions.empty() ||
                                 ed.fPrimaryAction || ed.fSecondaryAction || ed.fTertiaryAction;

    if (!requiresAction && !forcePopup)
    {
        eToast pendingToast;
        pendingToast.fEd = ed;
        pendingToast.fMsg = msg;
        pendingToast.fWid = nullptr;
        pendingToast.fDate = mBoard->date();
        pendingToast.fExpireFrame = 0;
        pendingToast.fQueued = false;
        mPendingToasts.push_back(pendingToast);
        return;
    }

    if (mMsgBox)
    {
        auto &smsg = prepend ? mSavedMsgs.emplace_front() : mSavedMsgs.emplace_back();
        smsg.fEd = ed;
        smsg.fMsg = msg;
        smsg.fForcePopup = forcePopup;
        return;
    }
    const auto msgb = new eMessageBox(window());
    mMsgBox = msgb;
    const bool wasPaused = mPaused;
    if (!wasPaused)
        switchPause();
    msgb->setHeight(height() / 3);
    msgb->setWidth(width() / 2);
    eAction a;
    if (ed.fChar)
    {
        const auto ch = ed.fChar;
        const auto tile = ed.fTile;
        a = [this, ch, tile]()
        {
            if (ch)
            {
                const auto t = ch->tile();
                viewTile(t);
            }
            else
            {
                viewTile(tile);
            }
        };
    }
    else if (ed.fTile)
    {
        const auto tile = ed.fTile;
        a = [this, tile]()
        {
            viewTile(tile);
        };
    }
    ed.fDate = mBoard->date();
    ed.fPlayerName = window()->leader();

    const auto requestActionTaken = std::make_shared<bool>(false);
    if (ed.fType == eMessageEventType::generalRequestGranted) {
        const auto wrapAction = [requestActionTaken](eAction& action) {
            if (!action) return;
            const auto oldAction = action;
            action = [requestActionTaken, oldAction]() {
                *requestActionTaken = true;
                oldAction();
            };
        };
        wrapAction(ed.fPrimaryAction);
        wrapAction(ed.fSecondaryAction);
        wrapAction(ed.fTertiaryAction);
        for (auto& a : ed.fCityConditionalActions) {
            wrapAction(a.second);
        }
    }

    const auto close = [this, wasPaused, ed, requestActionTaken, msgb]()
    {
        if (ed.fType == eMessageEventType::generalRequestGranted &&
            msgb->closable() &&
            !*requestActionTaken) {
            if (ed.fSecondaryAction) ed.fSecondaryAction();
            else if (ed.fTertiaryAction) ed.fTertiaryAction();
        }
        mMsgBox = nullptr;
        if (!wasPaused)
            switchPause();
        if (mSavedMsgs.empty())
            return;
        auto &msg = mSavedMsgs.front();
        showMessage(msg.fEd, msg.fMsg, false, msg.fForcePopup, true);
        mSavedMsgs.pop_front();
    };

    msgb->initialize(*mBoard, ed, a, close, msg);
    window()->execDialog(msgb, msgb->closable(), close, this);
    msgb->align(eAlignment::bottom | eAlignment::hcenter);
    msgb->setY(msgb->y() - mGm->width() / 10);
    msgb->setX(msgb->x() - mGm->width() / 2);
}

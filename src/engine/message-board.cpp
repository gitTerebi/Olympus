#include "game-board.h"

#include "gameEvents/egameevent.h"

void GameBoard::setMessageShower(const eMessageShower& msg)
{
    mMsgShower = msg;
}

void GameBoard::showMessage(eEventData& ed, const MessageType& msg)
{
    mMsgShower(ed, msg);
}

void GameBoard::respondToEvent(
    const int runtimeId,
    const int response,
    const eCityId city)
{
    const auto event = eventWithRuntimeId(runtimeId);
    if(!event) return;
    event->respond(response, city);
}

void GameBoard::addMessageLog(
    const eEventData& ed,
    const Message& msg,
    const eDate& date)
{
    auto& lm = mMessageLog.emplace_back();
    lm.fEd = ed;
    lm.fEd.fCloseResponse = -1;
    lm.fEd.fPrimaryResponse = -1;
    lm.fEd.fCityConditionalResponses.clear();
    lm.fEd.fSecondaryResponse = -1;
    lm.fEd.fTertiaryResponse = -1;
    lm.fEd.fType = eMessageEventType::common;
    lm.fMsg = msg;
    lm.fDate = date;
    lm.fRead = false;
    if(mMessageLog.size() > 50) {
        mMessageLog.erase(mMessageLog.begin());
    }
}

void GameBoard::setMessageLogRead(const int index)
{
    if(index < 0 || index >= static_cast<int>(mMessageLog.size())) return;
    mMessageLog[index].fRead = true;
}

void GameBoard::serializeMessageLog(SaveArchive& ar)
{
    int messageCount = ar.writing() ? static_cast<int>(mMessageLog.size()) : 0;
    ar.field("messageLog.count", messageCount);
    if(ar.reading()) {
        mMessageLog.clear();
        for(int i = 0; i < messageCount; i++) {
            eLoggedMessage lm;
            ar.archiveField(("message." + std::to_string(i)).c_str(),
                [&](SaveArchive& it) {
                    it.field("title", lm.fMsg.fTitle);
                    it.field("text", lm.fMsg.fText);
                    it.archiveField("date", [&](SaveArchive& dAr) {
                        lm.fDate.serialize(dAr);
                    });
                    it.field("playerName", lm.fEd.fPlayerName);
                    it.field("read", lm.fRead);
                });
            lm.fEd.fDate = lm.fDate;
            lm.fEd.fType = eMessageEventType::common;
            mMessageLog.push_back(lm);
        }
    } else {
        int i = 0;
        for(auto& lm : mMessageLog) {
            ar.archiveField(("message." + std::to_string(i++)).c_str(),
                [&](SaveArchive& it) {
                    it.field("title", lm.fMsg.fTitle);
                    it.field("text", lm.fMsg.fText);
                    it.archiveField("date", [&](SaveArchive& dAr) {
                        lm.fDate.serialize(dAr);
                    });
                    it.field("playerName", lm.fEd.fPlayerName);
                    it.field("read", lm.fRead);
                });
        }
    }
}

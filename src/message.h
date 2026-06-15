#ifndef MESSAGE_H
#define MESSAGE_H

#include <memory>
#include <string>

struct Message {
    std::string fTitle;
    std::string fText;
};

struct Reason {
    std::string fFull;
    std::string fCondensed;
};

struct MessageType {
    Message fFull;
    Message fCondensed;
};

struct EventMessageType : public MessageType {
    std::string fReason;
    std::string fNoReason;
};

struct ConsequenceMessageType : public MessageType {
    std::string fNoReason;
};

struct GameMessages {
    MessageType fBegin;
    MessageType fNoPart;
    MessageType fWon;
    MessageType fLost;
    MessageType fSecond;
};

#endif // MESSAGE_H

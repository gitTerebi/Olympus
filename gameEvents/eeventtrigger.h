#ifndef EEVENTTRIGGER_H
#define EEVENTTRIGGER_H

#include "pointers/eobject.h"

#include "engine/edate.h"

class eGameEvent;
class eWorldBoard;
enum class eCityId;

class eEventTrigger : public eStdSelfRef {
public:
    eEventTrigger(const eCityId cid,
                  const std::string& name,
                  eGameBoard& board);

    const std::string& name() const { return mName; }

    void trigger(eGameEvent& parent,
                 const eDate& date,
                 const std::string& reason);

    void loadResources() const;

    void write(eWriteStream& dst) const;
    void read(eReadStream& src);

    void addEvent(const stdsptr<eGameEvent>& e);
    void removeEvent(const stdsptr<eGameEvent>& e);
    const std::vector<stdsptr<eGameEvent>>& events() const
    { return mEvents; }
    int eventCount() const { return mEvents.size(); }

    eGameBoard* gameBoard() const { return &mBoard; }
    eWorldBoard* worldBoard() const;

    void setGameBoard(eGameBoard* const b);
    void setWorldBoard(eWorldBoard* const b);
private:
    const eCityId mCid;
    eGameBoard& mBoard;

    std::string mName;
    std::vector<stdsptr<eGameEvent>> mEvents;
};

#endif // EEVENTTRIGGER_H

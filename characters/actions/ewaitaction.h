#ifndef EWAITACTION_H
#define EWAITACTION_H

#include "echaracteraction.h"

#include "elimits.h"

class eSaveArchive;
class eJsonArchive;

class eWaitAction : public eCharacterAction {
public:
    eWaitAction(eCharacter* const c);

    void increment(const int by) override;

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;
    void serializeJson(eJsonArchive& ar) override;

    void setTime(const int t);
private:
    void serialize(eSaveArchive& ar);
    int mRemTime{__INT_MAX__};
};

#endif // EWAITACTION_H

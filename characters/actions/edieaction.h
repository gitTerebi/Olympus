#ifndef EDIEACTION_H
#define EDIEACTION_H

#include "echaracteraction.h"

class eSaveArchive;
class eJsonArchive;

class eDieAction : public eCharacterAction {
public:
    eDieAction(eCharacter* const c);

    void increment(const int by) override;

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;
    void serializeJson(eJsonArchive& ar) override;
private:
    void serialize(eSaveArchive& ar);
    int mTime = 0;
};

#endif // EDIEACTION_H

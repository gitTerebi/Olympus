#ifndef EBUILDACTION_H
#define EBUILDACTION_H

#include "echaracteraction.h"

#include "characters/eartisan.h"

class eSaveArchive;
class eJsonArchive;

class eBuildAction : public eCharacterAction {
public:
    eBuildAction(eCharacter* const c);

    void increment(const int by) override;

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;
    void serializeJson(eJsonArchive& ar) override;
private:
    void serialize(eSaveArchive& ar);
    int mSoundTime = 0;
    int mTime = 0;
};

#endif // EBUILDACTION_H

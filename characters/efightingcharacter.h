#ifndef EFIGHTINGCHARACTER_H
#define EFIGHTINGCHARACTER_H

class eReadStream;
class eWriteStream;
class eSaveArchive;
#include "fileIO/ejsonarchive.h"

class eCharacter;
class eFightingAction;

class eFightingCharacter {
public:
    eFightingCharacter(eCharacter* const c);

    void read(eReadStream& src);
    void write(eWriteStream& dst) const;

    eFightingAction* fightingAction() const;

    int range() const { return mRange; }
    void setRange(const int r) { mRange = r; }
    virtual void serializeJson(eJsonArchive& ar) {
        ar.field("mRange", mRange);
}

private:
    void serialize(eSaveArchive& ar);

    eCharacter* const mChar;

    int mRange = 0;
};

#endif // EFIGHTINGCHARACTER_H

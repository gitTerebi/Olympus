#ifndef EBUILDACTION_H
#define EBUILDACTION_H

#include "character-action.h"

#include "characters/eartisan.h"

class eSaveArchive;

class eBuildAction : public eCharacterAction {
public:
    eBuildAction(eCharacter* const c);

    void increment(const int by) override;
protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    int mSoundTime = 0;
    int mTime = 0;
};

#endif // EBUILDACTION_H

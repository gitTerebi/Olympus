#ifndef EBUILDACTION_H
#define EBUILDACTION_H

#include "character-action.h"

#include "characters/eartisan.h"

class SaveArchive;

class eBuildAction : public eCharacterAction {
public:
    eBuildAction(eCharacter* const c);

    void increment(const int by) override;
protected:
    void serializeFields(SaveArchive& ar) override;
private:
    int mSoundTime = 0;
    int mTime = 0;
};

#endif // EBUILDACTION_H

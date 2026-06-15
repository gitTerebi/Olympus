#ifndef EDIEACTION_H
#define EDIEACTION_H

#include "character-action.h"

class SaveArchive;

class eDieAction : public eCharacterAction {
public:
    eDieAction(eCharacter* const c);

    void increment(const int by) override;
protected:
    void serializeFields(SaveArchive& ar) override;
    void resumeFromSavedState() override;
private:
    int mTime = 0;
};

#endif // EDIEACTION_H

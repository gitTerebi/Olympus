#ifndef EWAITACTION_H
#define EWAITACTION_H

#include "character-action.h"

#include "game-limits.h"

class SaveArchive;

class eWaitAction : public eCharacterAction {
public:
    eWaitAction(eCharacter* const c);

    void increment(const int by) override;

    void setTime(const int t);
protected:
    void serializeFields(SaveArchive& ar) override;
private:
    int mRemTime{__INT_MAX__};
};

#endif // EWAITACTION_H

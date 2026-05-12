#ifndef EWOLFACTION_H
#define EWOLFACTION_H

#include "eanimalaction.h"

class eJsonArchive;

class eWolfAction : public eAnimalAction {
public:
    eWolfAction(eCharacter* const c,
                const int spawnerX, const int spawnerY);
    eWolfAction(eCharacter* const c);

    void increment(const int by) override;
    bool decide() override;

    void serializeJson(eJsonArchive& ar) override;
private:
    void goBack();
    void findPrey();

    mutable bool mHunting = false;
};

#endif // EWOLFACTION_H

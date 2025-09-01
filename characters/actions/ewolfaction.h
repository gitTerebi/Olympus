#ifndef EWOLFACTION_H
#define EWOLFACTION_H

#include "eanimalaction.h"

class eWolfAction : public eAnimalAction {
public:
    eWolfAction(eCharacter* const c,
                const int spawnerX, const int spawnerY);
    eWolfAction(eCharacter* const c);

    void increment(const int by) override;
    bool decide() override;

    void write(eWriteStream& dst) const override;
    void read(eReadStream& src) override;
private:
    void goBack();
    void findPrey();

    bool mHunting = false;
};

#endif // EWOLFACTION_H

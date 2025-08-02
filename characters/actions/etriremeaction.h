#ifndef ETRIREMEACTION_H
#define ETRIREMEACTION_H

#include "echaracteraction.h"

class eTriremeAction : public eCharacterAction {
public:
    eTriremeAction(eBuilding* const home,
                   eCharacter* const trireme);

    void increment(const int by) override;

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;
private:
    stdptr<eBuilding> mHome;
};

#endif // ETRIREMEACTION_H

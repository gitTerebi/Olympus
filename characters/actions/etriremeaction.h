#ifndef ETRIREMEACTION_H
#define ETRIREMEACTION_H

#include "efightingaction.h"

class eTriremeWharf;

class eTriremeAction : public eFightingAction {
public:
    eTriremeAction(eTriremeWharf * const home,
                   eCharacter* const trireme);

    bool decide() override;

    void increment(const int by) override;

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;

    void goHome() override;
    void goAbroad() override;

    eTriremeWharf* home() const;
private:
    stdptr<eTriremeWharf> mHome;
};

#endif // ETRIREMEACTION_H

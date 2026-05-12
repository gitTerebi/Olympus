#ifndef ETRIREMEACTION_H
#define ETRIREMEACTION_H

#include "efightingaction.h"

class eTriremeWharf;
class eSaveArchive;
class eJsonArchive;

class eTriremeAction : public eFightingAction {
public:
    eTriremeAction(eTriremeWharf * const home,
                   eCharacter* const trireme);

    bool decide() override;

    void increment(const int by) override;

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;
    void serializeJson(eJsonArchive& ar) override;

    void goHome() override;
    void goAbroad() override;

    eTriremeWharf* home() const;
private:
    void serialize(eSaveArchive& ar);

    eTile* exitPoint() const;

    stdptr<eTriremeWharf> mHome;
};

#endif // ETRIREMEACTION_H

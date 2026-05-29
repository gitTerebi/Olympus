#ifndef TRADER_H
#define TRADER_H

#include "ebasicpatroler.h"

class Trader : public eBasicPatroler {
public:
    Trader(GameBoard& board);

    void createFollowers();

    void setActionType(const eCharacterActionType t);
private:
    std::vector<stdsptr<eCharacter>> mFollowers;
};

#endif // TRADER_H

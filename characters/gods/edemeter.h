#ifndef EDEMETER_H
#define EDEMETER_H

#include "ebasicgod.h"

class eAphrodite : public eBasicGod {
public:
    eAphrodite(GameBoard& board);
};

class eAres : public eBasicGod {
public:
    eAres(GameBoard& board);
};

class eDemeter : public eBasicGod {
public:
    eDemeter(GameBoard& board);
};

class eHades : public eBasicGod {
public:
    eHades(GameBoard& board);
};

class eHephaestus : public eBasicGod {
public:
    eHephaestus(GameBoard& board);
};

class eHera : public eBasicGod {
public:
    eHera(GameBoard& board);
};

class ePoseidon : public eBasicGod {
public:
    ePoseidon(GameBoard& board);
};

#endif // EDEMETER_H

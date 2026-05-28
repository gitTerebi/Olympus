#ifndef ECALYDONIANBOAR_H
#define ECALYDONIANBOAR_H

#include "ebasicmonster.h"
#include "ewatermonster.h"

class eCalydonianBoar : public eBasicMonster {
public:
    eCalydonianBoar(GameBoard& board);
};

class eCerberus : public eBasicMonster {
public:
    eCerberus(GameBoard& board);
};

class eChimera : public eBasicMonster {
public:
    eChimera(GameBoard& board);
};

class eCyclops : public eBasicMonster {
public:
    eCyclops(GameBoard& board);
};

class eDragon : public eBasicMonster {
public:
    eDragon(GameBoard& board);
};

class eEchidna : public eBasicMonster {
public:
    eEchidna(GameBoard& board);
};

class eHarpies : public eBasicMonster {
public:
    eHarpies(GameBoard& board);
};

class eHector : public eBasicMonster {
public:
    eHector(GameBoard& board);
};

class eHydra : public eBasicMonster {
public:
    eHydra(GameBoard& board);
};

class eKraken : public eWaterMonster {
public:
    eKraken(GameBoard& board);
};

class eMaenads : public eBasicMonster {
public:
    eMaenads(GameBoard& board);
};

class eMedusa : public eBasicMonster {
public:
    eMedusa(GameBoard& board);
};

class eMinotaur : public eBasicMonster {
public:
    eMinotaur(GameBoard& board);
};

class eScylla : public eWaterMonster {
public:
    eScylla(GameBoard& board);
};

class eSphinx : public eBasicMonster {
public:
    eSphinx(GameBoard& board);
};

class eTalos : public eBasicMonster {
public:
    eTalos(GameBoard& board);
};

class eSatyr : public eBasicMonster {
public:
    eSatyr(GameBoard& board);
};

#endif // ECALYDONIANBOAR_H

#ifndef EACHILLES_H
#define EACHILLES_H

#include "ebasichero.h"

class eAchilles : public eBasicHero {
public:
    eAchilles(GameBoard& board);
};

class eAtalanta : public eBasicHero {
public:
    eAtalanta(GameBoard& board);
};

class eBellerophon : public eBasicHero {
public:
    eBellerophon(GameBoard& board);
};

class eHercules : public eBasicHero {
public:
    eHercules(GameBoard& board);
};

class eJason : public eBasicHero {
public:
    eJason(GameBoard& board);
};

class eOdysseus : public eBasicHero {
public:
    eOdysseus(GameBoard& board);
};

class ePerseus : public eBasicHero {
public:
    ePerseus(GameBoard& board);
};

class eTheseus : public eBasicHero {
public:
    eTheseus(GameBoard& board);
};

#endif // EACHILLES_H

#ifndef ACHILLES_H
#define ACHILLES_H

#include "ebasichero.h"

class Achilles : public eBasicHero {
public:
    Achilles(GameBoard& board);
};

class Atalanta : public eBasicHero {
public:
    Atalanta(GameBoard& board);
};

class Bellerophon : public eBasicHero {
public:
    Bellerophon(GameBoard& board);
};

class Hercules : public eBasicHero {
public:
    Hercules(GameBoard& board);
};

class Jason : public eBasicHero {
public:
    Jason(GameBoard& board);
};

class Odysseus : public eBasicHero {
public:
    Odysseus(GameBoard& board);
};

class Perseus : public eBasicHero {
public:
    Perseus(GameBoard& board);
};

class Theseus : public eBasicHero {
public:
    Theseus(GameBoard& board);
};

#endif // ACHILLES_H

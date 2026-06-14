#ifndef EATHENA_H
#define EATHENA_H

#include "extended-god.h"

class Apollo : public ExtendedGod {
public:
    Apollo(GameBoard& board);
};

class Artemis : public ExtendedGod {
public:
    Artemis(GameBoard& board);
};

class Athena : public ExtendedGod {
public:
    Athena(GameBoard& board);
};

class Atlas : public ExtendedGod {
public:
    Atlas(GameBoard& board);
};

class Zeus : public ExtendedGod {
public:
    Zeus(GameBoard& board);
};

#endif // EATHENA_H

#ifndef EDEMETER_H
#define EDEMETER_H

#include "basic-god.h"

class Aphrodite : public BasicGod {
public:
    Aphrodite(GameBoard& board);
};

class Ares : public BasicGod {
public:
    Ares(GameBoard& board);
};

class Demeter : public BasicGod {
public:
    Demeter(GameBoard& board);
};

class Hades : public BasicGod {
public:
    Hades(GameBoard& board);
};

class Hephaestus : public BasicGod {
public:
    Hephaestus(GameBoard& board);
};

class Hera : public BasicGod {
public:
    Hera(GameBoard& board);
};

class Poseidon : public BasicGod {
public:
    Poseidon(GameBoard& board);
};

#endif // EDEMETER_H

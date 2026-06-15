#ifndef entry_point_H
#define entry_point_H

#include "spawner.h"

#include "characters/actions/character-action-function.h"

class EntryPoint : public Spawner {
public:
    EntryPoint(const int id,
                eTile* const tile,
                GameBoard& board);

    void incTime(const int by) override;
    void spawn(eTile* const tile) override;

    // Popularity below this freezes immigration (Augustus-style sentiment gate).
    static int sImmigrationFreezePopularity;
};

class eSS_spawnFinish : public eCharActFunc {
public:
    eSS_spawnFinish(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::SS_spawnFinish) {}

    void call() override;
};

class RiverEntryPoint : public Banner {
public:
    RiverEntryPoint(const int id,
                     eTile* const tile,
                     GameBoard& board);
};

#endif // entry_point_H

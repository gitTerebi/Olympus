#ifndef eentrypoint_H
#define eentrypoint_H

#include "espawner.h"

#include "characters/actions/character-action-function.h"

class eEntryPoint : public eSpawner {
public:
    eEntryPoint(const int id,
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

class eRiverEntryPoint : public eBanner {
public:
    eRiverEntryPoint(const int id,
                     eTile* const tile,
                     GameBoard& board);
};

#endif // eentrypoint_H

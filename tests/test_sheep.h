#pragma once
// Integration test: sheep/carding shed production over N years at turbo speed.
// Call runSheepTest() after full engine init (numbers loaded, textures ready).
// Returns 0 on pass, 1 on fail.

#include <cstdio>
#include <vector>

#include "engine/e-game-board.h"
#include "engine/eworldboard.h"
#include "buildings/ecardingshed.h"
#include "buildings/eshepherbuildingbase.h"
#include "characters/esheep.h"
#include "engine/ecityid.h"

inline int runSheepTest(const int years = 5, const int speed = 80) {
    printf("\n=== test_sheep (integration) ===\n");
    printf("speed=%d  years=%d\n\n", speed, years);

    const auto cid = eCityId::city0;
    const auto pid = ePlayerId::player0;

    eWorldBoard world;
    eGameBoard board(world);
    board.initialize(30, 30);
    board.addCityToBoard(cid);
    board.updatePlayersOnBoard();

    // Place carding shed at (5,5) — 2x2
    eShepherBuildingBase* shed = nullptr;
    {
        const auto bc = [&board, cid, &shed]() {
            auto b = e::make_shared<eCardingShed>(board, cid);
            shed = b.get();
            return b;
        };
        const bool ok = board.buildBase(5, 5, 6, 6, bc, pid, cid, false);
        if (!ok || !shed) {
            printf("FAIL: could not place carding shed\n");
            return 1;
        }
    }

    // Full employment so effectiveness() == 1.0
    shed->setEmployed(shed->maxEmployees());

    // Place sheep at (10,10)
    {
        const auto tile = board.tile(10, 10);
        const bool ok = board.buildAnimal(tile, eBuildingType::sheep,
            [](eGameBoard& b) { return e::make_shared<eSheep>(b); },
            cid, pid, false);
        if (!ok) {
            printf("FAIL: could not place sheep\n");
            return 1;
        }
    }

    const int dayLen    = eNumbers::sDayLength;   // 350
    const int ticksYear = 365 * dayLen;            // 127750

    bool anyFail = false;
    for (int y = 0; y < years; y++) {
        const int prevProd = shed->producedThisYear();
        for (int t = 0; t < ticksYear; t += speed)
            board.incTime(speed);
        const int prod = shed->producedThisYear() - prevProd;
        const bool pass = prod >= 8;
        if (!pass) anyFail = true;
        printf("year %d: fleece=%d  %s\n", y + 1, prod, pass ? "PASS" : "FAIL <-- expected >=8");
    }

    printf("\n%s\n", anyFail ? "RESULT: FAIL" : "RESULT: PASS");
    return anyFail ? 1 : 0;
}

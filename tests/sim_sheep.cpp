// Sheep production simulator — no SDL, no game board, no GUI.
// Simulates the shepherd/sheep loop and counts fleeces delivered per year.
//
// Defaults match enumbers.cpp defaults:
//   day_length_i          = 350
//   sheep_max_groom_i     = 15
//   shepherd_goatherd_groom_time_i    = 813
//   shepherd_goatherd_collect_time_i  = 2000
//   shepherd_goatherd_wait_time_i     = 2000
//
// Speeds (egamewidget.h): {8, 12, 16, 32, 48, 64, 80}
//                          0.5x 0.75x 1x  2x  3x  4x  TURBO

#include <cstdio>
#include <cstdlib>
#include <cstring>

// ── configurable params ──────────────────────────────────────────────────────
static int DAY_LENGTH       = 350;
static int MAX_GROOM        = 15;
static int GROOM_TIME       = 813;
static int COLLECT_TIME     = 2000;
static int WAIT_TIME        = 2000;
static int YEARS_TO_SIM     = 5;

static const int SPEEDS[]   = {8, 12, 16, 32, 48, 64, 80};
static const char* LABELS[] = {"0.5x","0.75x","1x","2x","3x","4x","TURBO"};
static const int NUM_SPEEDS = 7;

// ── simulation ───────────────────────────────────────────────────────────────
// State machine mirroring eDomesticatedAnimal + eShepherdAction loop:
//   GROOM (repeat MAX_GROOM times) -> COLLECT -> DELIVER/WAIT -> GROOM ...
enum class Phase { Groom, Collect, Wait };

struct SheepSim {
    Phase phase  = Phase::Groom;
    int   timer  = GROOM_TIME;   // countdown
    int   grooms = 0;            // grooms done this cycle
    int   fleeces = 0;
};

// Returns fleeces collected after simulating ticks_total ticks,
// advancing by `by` per step.
static int simulate(int ticks_total, int by) {
    SheepSim s;
    s.timer = GROOM_TIME;
    int total = 0;

    for (int t = 0; t < ticks_total; t += by) {
        s.timer -= by;
        if (s.timer > 0) continue;

        switch (s.phase) {
        case Phase::Groom:
            s.grooms++;
            if (s.grooms >= MAX_GROOM) {
                // animal ready to shear — shepherd collects
                s.grooms = 0;
                s.phase  = Phase::Collect;
                s.timer  = COLLECT_TIME;
            } else {
                s.timer = GROOM_TIME;
            }
            break;
        case Phase::Collect:
            total++;          // fleece delivered
            s.phase = Phase::Wait;
            s.timer = WAIT_TIME;
            break;
        case Phase::Wait:
            s.phase = Phase::Groom;
            s.timer = GROOM_TIME;
            break;
        }
    }
    return total;
}

// ── main ─────────────────────────────────────────────────────────────────────
int main(int argc, char* argv[]) {
    // allow overriding params: sim_sheep [years] [max_groom] [groom_time] [collect_time] [wait_time]
    if (argc > 1) YEARS_TO_SIM  = atoi(argv[1]);
    if (argc > 2) MAX_GROOM     = atoi(argv[2]);
    if (argc > 3) GROOM_TIME    = atoi(argv[3]);
    if (argc > 4) COLLECT_TIME  = atoi(argv[4]);
    if (argc > 5) WAIT_TIME     = atoi(argv[5]);

    const int ticks_per_year = 365 * DAY_LENGTH;
    const int ticks_total    = YEARS_TO_SIM * ticks_per_year;

    printf("=== Sheep production sim ===\n");
    printf("Params: day_len=%d  max_groom=%d  groom_time=%d  collect_time=%d  wait_time=%d\n",
           DAY_LENGTH, MAX_GROOM, GROOM_TIME, COLLECT_TIME, WAIT_TIME);
    printf("Theoretical max/year: %.2f  (1 fleece per %d ticks)\n",
           (double)ticks_per_year / (MAX_GROOM * GROOM_TIME + COLLECT_TIME + WAIT_TIME),
           MAX_GROOM * GROOM_TIME + COLLECT_TIME + WAIT_TIME);
    printf("\n%-8s  %-6s  %-12s  %-10s\n", "Speed", "by", "Total fleeces", "Per year");
    printf("%-8s  %-6s  %-12s  %-10s\n", "--------", "------", "------------", "----------");

    bool anyFail = false;
    for (int i = 0; i < NUM_SPEEDS; i++) {
        const int by     = SPEEDS[i];
        const int total  = simulate(ticks_total, by);
        const double avg = (double)total / YEARS_TO_SIM;
        // PASS: expect ~8/year (allow ±1 for rounding at step boundaries)
        const bool pass  = avg >= 7.0 && avg <= 9.0;
        if (!pass) anyFail = true;
        printf("%-8s  %-6d  %-12d  %-10.2f  %s\n",
               LABELS[i], by, total, avg, pass ? "PASS" : "FAIL <-- expected ~8");
    }

    printf("\n%s\n", anyFail ? "RESULT: FAIL" : "RESULT: PASS");
    return anyFail ? 1 : 0;
}

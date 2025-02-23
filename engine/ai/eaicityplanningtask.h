#ifndef EAICITYPLANNINGTASK_H
#define EAICITYPLANNINGTASK_H

#include <vector>
#include <SDL2/SDL_rect.h>

#include "engine/etask.h"

enum class ePlayerId;
class eGameBoard;
class eAICityPlan;
struct eAICSpeciman;

class eAICityPlanningTask : public eTask {
public:
    eAICityPlanningTask(eGameBoard& board,
                        const SDL_Rect& bRect,
                        const ePlayerId pid,
                        const eCityId cid);
    ~eAICityPlanningTask();
protected:
    void run(eThreadBoard& data,
             const std::atomic_bool& interrupt);
    void finish();
private:
    eGameBoard& mBoard;
    const SDL_Rect mBRect;
    const ePlayerId mPid;
    std::vector<eAICSpeciman*> mPopulation;
    eAICityPlan* mPlan = nullptr;
    int mStage = 0;
    bool mInterrupted = false;
    int mBestGradeSince = 0;
};

#endif // EAICITYPLANNINGTASK_H

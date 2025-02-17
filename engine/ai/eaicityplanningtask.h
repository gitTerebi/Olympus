#ifndef EAICITYPLANNINGTASK_H
#define EAICITYPLANNINGTASK_H

#include "SDL2/SDL_rect.h"

#include "engine/etask.h"

enum class ePlayerId;
class eGameBoard;

class eAICityPlanningTask : public eTask {
public:
    eAICityPlanningTask(eGameBoard& board,
                        const SDL_Rect& bRect,
                        const ePlayerId pid,
                        const eCityId cid);

protected:
    void run(eThreadBoard& data);
    void finish();
private:
    eGameBoard& mBoard;
    const SDL_Rect mBRect;
    const ePlayerId mPid;
    void* mBest = nullptr;
    int mStage = 0;
};

#endif // EAICITYPLANNINGTASK_H

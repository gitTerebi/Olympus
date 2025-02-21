#ifndef EAICONNECTWITHROADTASK_H
#define EAICONNECTWITHROADTASK_H

#include <vector>
#include <SDL2/SDL_rect.h>

#include "engine/etask.h"

enum class ePlayerId;
class eGameBoard;
enum class eOrientation;

class eAIConnectWithRoadTask : public eTask {
public:
    using ePoints = std::vector<std::pair<SDL_Point, SDL_Point>>;
    eAIConnectWithRoadTask(eGameBoard& board,
                           const ePoints& pts,
                           const ePlayerId pid,
                           const eCityId cid);
protected:
    void run(eThreadBoard& data);
    void finish();
private:
    eGameBoard& mBoard;
    ePoints mPoints;
    const ePlayerId mPid;
    std::vector<eOrientation> mPath;
};

#endif // EAICONNECTWITHROADTASK_H

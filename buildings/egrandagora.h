#ifndef EGRANDAGORA_H
#define EGRANDAGORA_H

#include "eagorabase.h"

class eGrandAgora : public eAgoraBase {
public:
    eGrandAgora(const eAgoraOrientation o, eGameBoard& board,
                const eCityId cid);

    SDL_Point pt(const int id) const;
};

#endif // EGRANDAGORA_H

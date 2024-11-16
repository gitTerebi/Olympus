#ifndef EGRANDAGORA_H
#define EGRANDAGORA_H

#include "eagorabase.h"

class eGrandAgora : public eAgoraBase {
public:
    eGrandAgora(const eAgoraOrientation o, eGameBoard& board,
                const eCityId cid);

    SDL_Point pt(const int id) const;

    eAgoraOrientation orientation() const { return mO; }
private:
    const eAgoraOrientation mO;
};

#endif // EGRANDAGORA_H

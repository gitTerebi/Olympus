#ifndef ECOMMONAGORA_H
#define ECOMMONAGORA_H

#include "buildings/eagorabase.h"

class eCommonAgora : public eAgoraBase {
public:
    eCommonAgora(const eAgoraOrientation o, eGameBoard& board,
                 const eCityId cid);

    SDL_Point pt(const int id) const;
};

#endif // ECOMMONAGORA_H

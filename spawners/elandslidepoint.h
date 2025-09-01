#ifndef ELANDSLIDEPOINT_H
#define ELANDSLIDEPOINT_H

#include "ebanner.h"

class eLandSlidePoint : public eBanner {
public:
    eLandSlidePoint(const int id,
                    eTile* const tile,
                    eGameBoard& board);
};

#endif // ELANDSLIDEPOINT_H

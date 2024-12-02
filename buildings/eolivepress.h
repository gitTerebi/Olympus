#ifndef EOLIVEPRESS_H
#define EOLIVEPRESS_H

#include "eprocessingbuilding.h"

class eOlivePress : public eProcessingBuilding {
public:
    eOlivePress(eGameBoard& board, const eCityId cid);
};

#endif // EOLIVEPRESS_H

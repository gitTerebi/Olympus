#ifndef EGIFTFROMEVENT_H
#define EGIFTFROMEVENT_H

#include "eresourcegrantedeventbase.h"

class eGiftFromEvent : public eResourceGrantedEventBase {
public:
    eGiftFromEvent(const eGameEventBranch branch,
                   eGameBoard& board);

    std::string longName() const override;
};

#endif // EGIFTFROMEVENT_H

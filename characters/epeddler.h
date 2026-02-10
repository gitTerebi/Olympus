#ifndef EPEDDLER_H
#define EPEDDLER_H

#include "ebasicpatroler.h"

class eAgoraBase;

class ePeddler : public eBasicPatroler {
public:
    ePeddler(eGameBoard& board);

    void provideToBuilding(eBuilding* const b);

    void setAgora(eAgoraBase* const a);
    eAgoraBase* agora() const;
private:
    stdptr<eAgoraBase> mAgora;
};

#endif // EPEDDLER_H

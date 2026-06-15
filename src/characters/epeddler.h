#ifndef EPEDDLER_H
#define EPEDDLER_H

#include "ebasicpatroler.h"

class eAgoraBase;
class SaveArchive;

class ePeddler : public eBasicPatroler {
public:
    ePeddler(GameBoard& board);

    void provideToBuilding(eBuilding* const b) override;

    void setAgora(eAgoraBase* const a);
    eAgoraBase* agora() const;

protected:
    void serializeFields(SaveArchive& ar) override;
private:
    stdptr<eAgoraBase> mAgora;
};

#endif // EPEDDLER_H

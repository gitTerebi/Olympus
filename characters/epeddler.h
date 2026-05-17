#ifndef EPEDDLER_H
#define EPEDDLER_H

#include "ebasicpatroler.h"

class eAgoraBase;
class eSaveArchive;

class ePeddler : public eBasicPatroler {
public:
    ePeddler(eGameBoard& board);

    void provideToBuilding(eBuilding* const b) override;

    void setAgora(eAgoraBase* const a);
    eAgoraBase* agora() const;

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;
private:
    void serialize(eSaveArchive& ar);

    stdptr<eAgoraBase> mAgora;
};

#endif // EPEDDLER_H

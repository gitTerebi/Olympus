#ifndef ESETTLER_H
#define ESETTLER_H

#include "ebasicpatroler.h"

class eSettler : public eBasicPatroler {
public:
    eSettler(eGameBoard& board);

    void setEmigrant(const bool e);
    bool emigrant() const { return mEmigrant; }

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;
private:
    bool mEmigrant = false;
};

#endif // ESETTLER_H

#ifndef ESETTLER_H
#define ESETTLER_H

#include "ebasicpatroler.h"

class eSaveArchive;

class eSettler : public eBasicPatroler {
public:
    eSettler(eGameBoard& board);

    void setEmigrant(const bool e);
    bool emigrant() const { return mEmigrant; }

protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    bool mEmigrant = false;
};

#endif // ESETTLER_H

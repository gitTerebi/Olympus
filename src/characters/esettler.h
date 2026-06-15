#ifndef ESETTLER_H
#define ESETTLER_H

#include "ebasicpatroler.h"

class SaveArchive;

class eSettler : public eBasicPatroler {
public:
    eSettler(GameBoard& board);

    void setEmigrant(const bool e);
    bool emigrant() const { return mEmigrant; }

protected:
    void serializeFields(SaveArchive& ar) override;
private:
    bool mEmigrant = false;
};

#endif // ESETTLER_H

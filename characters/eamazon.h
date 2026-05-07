#ifndef EAMAZON_H
#define EAMAZON_H

#include "esoldier.h"

class eSaveArchive;

class eAmazon : public eSoldier {
public:
    eAmazon(eGameBoard& board);

    bool isArcher() const { return mIsArcher; }
    void setIsArcher(const bool a);

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;
private:
    void serialize(eSaveArchive& ar);

    bool mIsArcher = false;
};

#endif // EAMAZON_H

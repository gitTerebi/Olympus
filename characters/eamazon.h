#ifndef EAMAZON_H
#define EAMAZON_H

#include "esoldier.h"

class eSaveArchive;

class eAmazon : public eSoldier {
public:
    eAmazon(eGameBoard& board);

    bool isArcher() const { return mIsArcher; }
    void setIsArcher(const bool a);

protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    bool mIsArcher = false;
};

#endif // EAMAZON_H

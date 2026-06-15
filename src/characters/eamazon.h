#ifndef EAMAZON_H
#define EAMAZON_H

#include "esoldier.h"

class SaveArchive;

class eAmazon : public eSoldier {
public:
    eAmazon(GameBoard& board);

    bool isArcher() const { return mIsArcher; }
    void setIsArcher(const bool a);

protected:
    void serializeFields(SaveArchive& ar) override;
private:
    bool mIsArcher = false;
};

#endif // EAMAZON_H

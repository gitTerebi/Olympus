#ifndef ETRAILER_H
#define ETRAILER_H

#include "echaracter.h"
#include "ecarttransporter.h"

class SaveArchive;

class eTrailer : public eCharacter {
public:
    eTrailer(GameBoard& board);

    std::shared_ptr<Texture>
        getTexture(const eTileSize size) const override;

    void setFollow(eCartTransporter* const c) { mFollow = c; }

    void setBig(const bool b) { mIsBig = b; }

protected:
    void serializeFields(SaveArchive& ar) override;
private:
    stdptr<eCartTransporter> mFollow;

    bool mIsBig = false;

    mutable int mResCount = 0;
    mutable eResourceType mResType = eResourceType::marble;
};

#endif // ETRAILER_H

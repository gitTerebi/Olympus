#ifndef ERACINGHORSE_H
#define ERACINGHORSE_H

#include "missiles/emissile.h"

class eSaveArchive;

class eRacingHorse : public eMissile {
public:
    eRacingHorse(GameBoard& board, const int id,
                 const std::vector<ePathPoint>& path = {});
    eRacingHorse(GameBoard& board);

    std::shared_ptr<eTexture>
    getTexture(const eTileSize size) const override;

protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    int mId;
};

#endif // ERACINGHORSE_H

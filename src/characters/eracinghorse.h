#ifndef ERACINGHORSE_H
#define ERACINGHORSE_H

#include "missiles/missile.h"

class SaveArchive;

class eRacingHorse : public Missile {
public:
    eRacingHorse(GameBoard& board, const int id,
                 const std::vector<PathPoint>& path = {});
    eRacingHorse(GameBoard& board);

    std::shared_ptr<Texture>
    getTexture(const eTileSize size) const override;

protected:
    void serializeFields(SaveArchive& ar) override;
private:
    int mId;
};

#endif // ERACINGHORSE_H

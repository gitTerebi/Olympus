#ifndef GOD_MISSILE_H
#define GOD_MISSILE_H

#include "missile.h"

class SaveArchive;

class GodMissile : public Missile {
public:
    GodMissile(GameBoard& board,
                const std::vector<PathPoint>& path = {});

    std::shared_ptr<Texture> getTexture(const eTileSize size) const override;

    void setTexture(const eCharacterType ct,
                    const eCharacterActionType cat);

protected:
    void serializeFields(SaveArchive& ar) override;
private:
    eCharacterType mCharType;
    eCharacterActionType mActionType;
};

#endif // GOD_MISSILE_H

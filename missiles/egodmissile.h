#ifndef EGODMISSILE_H
#define EGODMISSILE_H

#include "emissile.h"

class eSaveArchive;

class eGodMissile : public eMissile {
public:
    eGodMissile(eGameBoard& board,
                const std::vector<ePathPoint>& path = {});

    std::shared_ptr<eTexture> getTexture(const eTileSize size) const override;

    void setTexture(const eCharacterType ct,
                    const eCharacterActionType cat);

protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    eCharacterType mCharType;
    eCharacterActionType mActionType;
};

#endif // EGODMISSILE_H

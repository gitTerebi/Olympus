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

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;
    void serialize(eSaveArchive& ar);
    void serializeJson(eJsonArchive& ar, eGameBoard& board) override;
private:
    eCharacterType mCharType;
    eCharacterActionType mActionType;
};

#endif // EGODMISSILE_H

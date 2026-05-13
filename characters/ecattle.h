#ifndef ECATTLE_H
#define ECATTLE_H

#include "echaracter.h"

class eSaveArchive;

class eCattle : public eCharacter {
public:
    eCattle(eGameBoard& board, const eCharacterType type);

    std::shared_ptr<eTexture>
    getTexture(const eTileSize size) const override;

    void incTime(const int by) override;

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;
    void serializeJson(eJsonArchive& ar) override;
private:
    void serialize(eSaveArchive& ar);

    bool mature();
    bool shouldBecomeBull() const;
    eTile* getSpawnerTile() const;

    int mId;
    int mMatureWait = 0;

    static int sId;
};

#endif // ECATTLE_H

#ifndef ECATTLE_H
#define ECATTLE_H

#include "echaracter.h"

class eSaveArchive;

class eCattle : public eCharacter {
public:
    eCattle(GameBoard& board, const eCharacterType type);

    std::shared_ptr<eTexture>
    getTexture(const eTileSize size) const override;

    void incTime(const int by) override;

protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    bool mature();
    bool shouldBecomeBull() const;
    eTile* getSpawnerTile() const;

    int mId;
    int mMatureWait = 0;

    static int sId;
};

#endif // ECATTLE_H

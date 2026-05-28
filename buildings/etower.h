#ifndef ETOWER_H
#define ETOWER_H

#include "eemployingbuilding.h"

#include "characters/earcher.h"

class eSaveArchive;

enum class eTowerEmploymentState {
    shutdown,
    noPalace,
    noRoad,
    available
};

class eTower : public eEmployingBuilding {
public:
    eTower(GameBoard& board, const eCityId cid);
    ~eTower();

    std::shared_ptr<eTexture>
    getTexture(const eTileSize size) const override;
    std::vector<eOverlay>
    getOverlays(const eTileSize size) const override;

    void timeChanged(const int by) override;

    bool enabled() const override;
    void setEmployed(const int e) override;

    bool spawn();

    void setDeleteArchers(const bool d) { mDeleteArchers = d; }

    eTowerEmploymentState employmentState(const bool mManTowers, const bool hasPalace) const;
protected:
    void serializeFields(eSaveArchive& ar) override;
private:

    bool mDeleteArchers = true;

    int mMissile = 0;
    int mRangeAttack = 0;
    int mAttackTime = 0;
    bool mAttack = false;
    eOrientation mAttackOrientation{eOrientation::topRight};
    stdptr<eCharacter> mAttackTarget;

    int mSpawnTime = 0;

    stdptr<eArcher> mArcher;
};

#endif // ETOWER_H

#ifndef ETEMPLEALTARBUILDING_H
#define ETEMPLEALTARBUILDING_H

#include "esanctbuilding.h"

#include <functional>

class eSaveArchive;

enum class eSacrifice {
    none,
    sheep,
    goods,
    bull
};

class eTempleAltarBuilding : public eSanctBuilding {
public:
    eTempleAltarBuilding(GameBoard& board, const eCityId cid);

    std::shared_ptr<eTexture> getTexture(const eTileSize) const override { return nullptr; }

    std::vector<eOverlay>
    getOverlays(const eTileSize size) const override;

    void timeChanged(const int by) override;

    void startSacrifice(const eSacrifice s);
    bool sacrificing() const { return mSacrifice != eSacrifice::none; }
    bool priestOut() const { return mPriestOut; }
    int sacrificeDaysLeft() const;  // 0 if not sacrificing

    void setOnSacrificeComplete(std::function<void()> cb) { mOnSacrificeComplete = std::move(cb); }

    int id() const { return mId; }
    void setId(const int i) { mId = i % 2; }
protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    eSacrifice mSacrifice = eSacrifice::none;
    int mId = 0;
    int mSacrificeTime = 0;
    int mSpawnTimer = 0;
    bool mPriestOut = false;
    std::function<void()> mOnSacrificeComplete;
};

#endif // ETEMPLEALTARBUILDING_H

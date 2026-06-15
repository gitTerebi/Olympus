#ifndef ECORRAL_H
#define ECORRAL_H

#include "eresourcebuildingbase.h"

class SaveArchive;

class eCorral : public eResourceBuildingBase {
public:
    eCorral(GameBoard& board, const eCityId cid);

    std::shared_ptr<Texture> getTexture(const eTileSize size) const override;
    std::vector<Overlay> getOverlays(const eTileSize size) const override;

    void timeChanged(const int by) override;

    int cattleCount() const { return mNCattle; }
    void addCattle();
    bool noCattle() const { return mNoCattle; }
    void setNoCattle(const bool c);
protected:
    void serializeFields(SaveArchive& ar) override;
private:
    bool killCattle();

    bool takeCattle();
    void replaceCattle();

    bool mNoCattle = true;
    double mProcessing = 0;
    int mNCattle = 0;
    double mTakeWait = 0;
    double mKillWait = 0;
    double mReplaceWait = 0;
};

#endif // ECORRAL_H

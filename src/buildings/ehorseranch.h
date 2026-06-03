#ifndef EHORSERANCH_H
#define EHORSERANCH_H

#include "eemployingbuilding.h"

class HorseRanchEnclosure;
class eSaveArchive;

class eHorseRanch : public eEmployingBuilding {
public:
    eHorseRanch(GameBoard& board, const eCityId cid);
    ~eHorseRanch();

    void erase() override;

    std::shared_ptr<eTexture> getTexture(const eTileSize size) const override;
    std::vector<eOverlay> getOverlays(const eTileSize size) const override;

    void timeChanged(const int by) override;

    int add(const eResourceType type, const int count) override;
    int take(const eResourceType type, const int count) override;

    int count(const eResourceType type) const override;
    int spaceLeft(const eResourceType type) const override;

    std::vector<eCartTask> cartTasks() const override;

    int provide(const eProvide p, const int n) override;

    int wheat() const { return mWheat; }

    HorseRanchEnclosure* enclosure() const { return mEnclosure; }
    void setEnclosure(HorseRanchEnclosure* const e);

    int horseCount() const;
    bool takeHorse();

protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    int mWheat = 0;
    int mWheatTime = 0;
    int mHorseTime = 0;
    HorseRanchEnclosure* mEnclosure = nullptr;

    stdptr<eCartTransporter> mTakeCart;
};

#endif // EHORSERANCH_H

#ifndef HORSE_RANCH_ENCLOSURE_H
#define HORSE_RANCH_ENCLOSURE_H

#include "ebuildingwithresource.h"

class Horse;
class eHorseRanch;
class eSaveArchive;

class HorseRanchEnclosure : public eBuildingWithResource {
public:
    HorseRanchEnclosure(GameBoard& board, const eCityId cid);
    ~HorseRanchEnclosure();

    void erase() override;

    int provide(const eProvide p, const int n) override;

    std::shared_ptr<eTexture> getTexture(const eTileSize size) const override;
    std::vector<eOverlay> getOverlays(const eTileSize size) const override;

    void timeChanged(const int by) override;

    int count(const eResourceType type) const override;
    int take(const eResourceType type, const int count) override;

    int horseCount() const { return mHorses.size(); }

    bool spawnHorse();
    bool takeHorse();

    void setRanch(eHorseRanch* const ranch);
    eHorseRanch* ranch() const { return mRanch; }

protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    std::vector<stdsptr<Horse>> mHorses;

    eHorseRanch* mRanch = nullptr;
};

#endif // HORSE_RANCH_ENCLOSURE_H

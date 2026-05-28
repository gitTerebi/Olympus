#ifndef EHORSERANCHENCLOSURE_H
#define EHORSERANCHENCLOSURE_H

#include "ebuildingwithresource.h"

class eHorse;
class eHorseRanch;
class eSaveArchive;

class eHorseRanchEnclosure : public eBuildingWithResource {
public:
    eHorseRanchEnclosure(GameBoard& board, const eCityId cid);
    ~eHorseRanchEnclosure();

    void erase() override;

    int provide(const eProvide p, const int n) override;

    std::shared_ptr<eTexture> getTexture(const eTileSize size) const override;
    std::vector<eOverlay> getOverlays(const eTileSize size) const override;

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
    std::vector<stdsptr<eHorse>> mHorses;

    eHorseRanch* mRanch = nullptr;
};

#endif // EHORSERANCHENCLOSURE_H

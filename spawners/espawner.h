#ifndef ESPAWNER_H
#define ESPAWNER_H

#include "ebanner.h"

class eSaveArchive;

class eSpawner : public eBanner {
public:
    eSpawner(const eBannerTypeS type,
             const int id,
             eTile* const tile,
             const int maxCount,
             const int spawnFreq,
             eGameBoard& board);
    virtual ~eSpawner();

    virtual void spawn(eTile* const tile) = 0;

    virtual void incTime(const int by);

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;

    void decCount();

    virtual void spawnMax();

    int count() const { return mCount; }
    int maxCount() const { return mMaxCount; }

    void setSpawnPeriod(const int p);
    void disableSpawning();
private:
    void serialize(eSaveArchive& ar);
    const int mMaxCount;
    int mSpawnPeriod = 100;

    int mCount = 0;
    int mTime = 0;
    bool mSpawningEnabled = true;
};

#endif // ESPAWNER_H

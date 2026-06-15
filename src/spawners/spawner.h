#ifndef SPAWNER_H
#define SPAWNER_H

#include "banner.h"

class SaveArchive;

class Spawner : public Banner {
public:
    Spawner(const BannerTypeS type,
             const int id,
             eTile* const tile,
             const int maxCount,
             const int spawnFreq,
             GameBoard& board);
    virtual ~Spawner();

    virtual void spawn(eTile* const tile) = 0;

    virtual void incTime(const int by);

    void serialize(SaveArchive& ar) override;

    void decCount();

    virtual void spawnMax();

    int count() const { return mCount; }
    int maxCount() const { return mMaxCount; }

    void setSpawnPeriod(const int p);
    void disableSpawning();
private:
    const int mMaxCount;
    int mSpawnPeriod = 100;

    int mCount = 0;
    int mTime = 0;
    bool mSpawningEnabled = true;
};

#endif // SPAWNER_H

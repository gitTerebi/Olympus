#ifndef ETIDALWAVEEVENT_H
#define ETIDALWAVEEVENT_H

#include "egameevent.h"

class eTidalWaveEvent : public eGameEvent {
public:
    eTidalWaveEvent(const eCityId cid,
                    const eGameEventBranch branch,
                    eGameBoard& board);

    void trigger() override;
    std::string longName() const override;

    void write(eWriteStream& dst) const override;
    void read(eReadStream& src) override;

    int disasterPoint() const { return mDisasterPoint; }
    void setDisasterPoint(const int p) { mDisasterPoint = p; }

    bool permanent() const { return mPermanent; }
    void setPermanent(const bool p) { mPermanent = p; }
private:
    int mDisasterPoint = 0;
    bool mPermanent = false;
};

#endif // ETIDALWAVEEVENT_H

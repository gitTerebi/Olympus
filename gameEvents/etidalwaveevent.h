#ifndef ETIDALWAVEEVENT_H
#define ETIDALWAVEEVENT_H

#include "egameevent.h"
#include "epointeventvalue.h"
#include "egodeventvalue.h"
#include "egodreasoneventvalue.h"

class eTidalWaveEvent : public eGameEvent,
                        public ePointEventValue,
                        public eGodEventValue,
                        public eGodReasonEventValue {
public:
    eTidalWaveEvent(const eCityId cid,
                    const eGameEventBranch branch,
                    eGameBoard& board);

    void trigger() override;
    std::string longName() const override;

    void write(eWriteStream& dst) const override;
    void read(eReadStream& src) override;

    void loadResources() const override;

    bool permanent() const { return mPermanent; }
    void setPermanent(const bool p) { mPermanent = p; }
private:
    bool mPermanent = false;
};

#endif // ETIDALWAVEEVENT_H

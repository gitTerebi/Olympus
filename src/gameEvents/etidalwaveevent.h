#ifndef ETIDALWAVEEVENT_H
#define ETIDALWAVEEVENT_H

#include "egameevent.h"
#include "epointeventvalue.h"
#include "gods/egodeventvalue.h"
#include "gods/egodreasoneventvalue.h"

class eSaveArchive;

class eTidalWaveEvent : public eGameEvent,
                        public ePointEventValue,
                        public eGodEventValue,
                        public eGodReasonEventValue {
public:
    eTidalWaveEvent(const eCityId cid,
                    const eGameEventBranch branch,
                    GameBoard& board);

    void trigger() override;
    std::string longName() const override;

    void loadResources() const override;

    bool permanent() const { return mPermanent; }
    void setPermanent(const bool p) { mPermanent = p; }
protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    bool mPermanent = false;
};

#endif // ETIDALWAVEEVENT_H

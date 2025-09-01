#ifndef ELANDSLIDEEVENT_H
#define ELANDSLIDEEVENT_H

#include "egameevent.h"
#include "epointeventvalue.h"

class eLandSlideEvent : public eGameEvent,
                        public ePointEventValue {
public:
    eLandSlideEvent(const eCityId cid,
                    const eGameEventBranch branch,
                    eGameBoard& board);

    void trigger() override;
    std::string longName() const override;

    void write(eWriteStream& dst) const override;
    void read(eReadStream& src) override;

    void loadResources() const override;
};

#endif // ELANDSLIDEEVENT_H

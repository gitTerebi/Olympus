#ifndef ELANDSLIDEEVENT_H
#define ELANDSLIDEEVENT_H

#include "egameevent.h"
#include "epointeventvalue.h"

class eSaveArchive;

class eLandSlideEvent : public eGameEvent,
                        public ePointEventValue {
public:
    eLandSlideEvent(const eCityId cid,
                    const eGameEventBranch branch,
                    eGameBoard& board);

    void trigger() override;
    std::string longName() const override;

    void loadResources() const override;
protected:
    void serializeFields(eSaveArchive& ar) override;
};

#endif // ELANDSLIDEEVENT_H

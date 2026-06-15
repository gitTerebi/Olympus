#ifndef ELANDSLIDEEVENT_H
#define ELANDSLIDEEVENT_H

#include "egameevent.h"
#include "epointeventvalue.h"

class SaveArchive;

class eLandSlideEvent : public eGameEvent,
                        public ePointEventValue {
public:
    eLandSlideEvent(const eCityId cid,
                    const eGameEventBranch branch,
                    GameBoard& board);

    void trigger() override;
    std::string longName() const override;

    void loadResources() const override;
protected:
    void serializeFields(SaveArchive& ar) override;
};

#endif // ELANDSLIDEEVENT_H

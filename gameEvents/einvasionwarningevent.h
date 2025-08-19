#ifndef EINVASIONWARNINGEVENT_H
#define EINVASIONWARNINGEVENT_H

#include "egameevent.h"

enum class eInvasionWarningType {
    warning36,
    warning24,
    warning12,
    warning6,
    warning1
};

class eInvasionWarningEvent : public eGameEvent {
public:
    eInvasionWarningEvent(const eCityId cid,
                          const eGameEventBranch branch,
                          eGameBoard& board);

    void initialize(const eInvasionWarningType type);

    void trigger() override;
    std::string longName() const override;

    void write(eWriteStream& dst) const override ;
    void read(eReadStream& src) override;
private:
    eInvasionWarningType mType;
};

#endif // EINVASIONWARNINGEVENT_H

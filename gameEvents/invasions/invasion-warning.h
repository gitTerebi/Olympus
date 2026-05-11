#ifndef INVASION_WARNING_H
#define INVASION_WARNING_H

#include "../ewarning.h"

enum class eInvasionWarningType {
    warningInitial,
    warning24,
    warning12,
    warning6,
    warning1
};

class eInvasionWarning : public eWarning {
public:
    eInvasionWarning(const int warningDays,
                     eGameEvent& parent,
                     const eCityId cid,
                     eGameBoard& board,
                     const eInvasionWarningType type);

    void trigger() override;
private:
    const eInvasionWarningType mType;
};

#endif // INVASION_WARNING_H

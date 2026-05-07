#ifndef EFILEFORMAT_H
#define EFILEFORMAT_H

namespace eFileFormat {
    enum {
        initial,
        settlerEmigrant,
        cartTarget,
        ruinsOrigin,
        ruinsSavedBuilding,
        yearlyProduction,
        ruinsRestoreBundle,

        nextVersion
    };

    const int version = nextVersion - 1;

    inline bool hasYearlyProductionData(const int version) {
        // Yearly production fields were written before the version enum was
        // bumped for them, so released v2-v4 saves already contain the data.
        return version >= cartTarget;
    }
}

#endif // EFILEFORMAT_H

#ifndef EFILEFORMAT_H
#define EFILEFORMAT_H

namespace eFileFormat {
    enum Version {
        initial,
        settlerEmigrant,
        cartTarget,
        ruinsOrigin,
        ruinsSavedBuilding,
        yearlyProduction,
        ruinsRestoreBundle,
        stockpiledResources,

        nextVersion
    };

    const int version = nextVersion - 1;

    // Any newly serialized field must append a Version entry above
    // nextVersion, add a hasX() helper here, and use that helper in every
    // matching read path with an old-save default.
    //
    // Never read a newly added field unconditionally.

    inline bool hasSettlerEmigrant(const int version) {
        return version >= settlerEmigrant;
    }

    inline bool hasCartTarget(const int version) {
        return version >= cartTarget;
    }

    inline bool hasRuinsOrigin(const int version) {
        return version >= ruinsOrigin;
    }

    inline bool hasRuinsSavedBuilding(const int version) {
        return version >= ruinsSavedBuilding;
    }

    inline bool hasYearlyProductionData(const int version) {
        // Yearly production fields were written before the version enum was
        // bumped for them, so released v2-v4 saves already contain the data.
        return version >= cartTarget;
    }

    inline bool hasRuinsRestoreBundle(const int version) {
        return version >= ruinsRestoreBundle;
    }

    inline bool hasStockpiledResources(const int version) {
        return version >= stockpiledResources;
    }

}

#endif // EFILEFORMAT_H

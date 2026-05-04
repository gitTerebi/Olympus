#ifndef EFILEFORMAT_H
#define EFILEFORMAT_H

namespace eFileFormat {
    enum {
        initial,
        settlerEmigrant,
        cartTarget,
        ruinsOrigin,
        ruinsSavedBuilding,

        nextVersion
    };

    const int version = nextVersion - 1;
}

#endif // EFILEFORMAT_H

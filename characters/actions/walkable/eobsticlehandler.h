#ifndef EOBSTICLEHANDLER_H
#define EOBSTICLEHANDLER_H

#include "pointers/estdpointer.h"
#include "fileIO/estreams.h"
#include "fileIO/ejsonarchive.h"

class eTile;

enum class eObsticleHandlerType {
    monster,
    god,
    soldier
};

class eObsticleHandler {
public:
    eObsticleHandler(eGameBoard& board,
                     const eObsticleHandlerType type) :
        mBoard(board), mType(type) {}

    virtual bool handle(eTile* const tile) = 0;

    virtual void read(eReadStream&) {}
    virtual void write(eWriteStream&) const {}

    eGameBoard& board() const { return mBoard; }
    eObsticleHandlerType type() const { return mType; }

    static stdsptr<eObsticleHandler> sCreate(
            eGameBoard& board,
            const eObsticleHandlerType type);
    virtual void serializeJson(eJsonArchive& ar) {
        // empty read — no fields to serialize
    }

private:
    eGameBoard& mBoard;
    const eObsticleHandlerType mType;
};

#endif // EOBSTICLEHANDLER_H

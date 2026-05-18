#include "ewildanimal.h"

#include "spawners/espawner.h"
#include "fileIO/esavearchive.h"

eWildAnimal::eWildAnimal(eGameBoard& board,
                         const eCharTexs charTexs,
                         const eCharacterType type) :
    eAnimal(board, charTexs, type) {}

eWildAnimal::~eWildAnimal() {
    if(mSpawner) mSpawner->decCount();
}

void eWildAnimal::read(eReadStream& src) {
    eAnimal::read(src);
    eSaveArchive ar(src);
    serialize(ar);
}

void eWildAnimal::write(eWriteStream& dst) const {
    eAnimal::write(dst);
    eSaveArchive ar(dst);
    const_cast<eWildAnimal*>(this)->serialize(ar);
}

void eWildAnimal::serialize(eSaveArchive& ar) {
    ar.payloadField("spawner",
        [this](eWriteStream& dst) { dst.writeBanner(mSpawner); },
        [this](eReadStream& src) {
            src.readBanner(&getBoard(), [this](eBanner* const b) {
                mSpawner = static_cast<eSpawner*>(b);
            });
        });
}

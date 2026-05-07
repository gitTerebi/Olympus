#include "egameboard.h"

#include "spawners/ebanner.h"
#include "fileIO/ebuildingwriter.h"
#include "fileIO/esavearchive.h"
#include "einvasionhandler.h"
#include "missiles/emissile.h"
#include "gameEvents/egameevent.h"
#include "eplague.h"

void eGameBoard::write(eWriteStream& dst) const {
    eSaveArchive ar(dst);
    const_cast<eGameBoard*>(this)->serialize(ar);
}

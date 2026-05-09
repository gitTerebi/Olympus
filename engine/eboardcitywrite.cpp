#include "eboardcity.h"

#include "e-invasion-handler.h"
#include "engine/eplague.h"
#include "engine/emilitaryaid.h"
#include "gameEvents/emonsterinvasioneventbase.h"
#include "gameEvents/egameevent.h"
#include "buildings/ehippodrome.h"
#include "fileIO/esavearchive.h"

void eBoardCity::write(eWriteStream& dst) const {
    eSaveArchive ar(dst);
    const_cast<eBoardCity*>(this)->serialize(ar);
}

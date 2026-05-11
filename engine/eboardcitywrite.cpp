#include "eboardcity.h"

#include "gameEvents/invasions/invasion-handler.h"
#include "engine/eplague.h"
#include "engine/emilitaryaid.h"
#include "gameEvents/invasions/monster-invasion-event-base.h"
#include "gameEvents/egameevent.h"
#include "buildings/ehippodrome.h"
#include "fileIO/esavearchive.h"

void eBoardCity::write(eWriteStream& dst) const {
    eSaveArchive ar(dst);
    const_cast<eBoardCity*>(this)->serialize(ar);
}

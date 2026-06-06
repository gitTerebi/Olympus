#include "message-action-refresh.h"

#include "engine/game-board.h"
#include "gameEvents/egameevent.h"

void refreshMessageActions(GameBoard* const board, eEventData& ed)
{
    if(!board) return;
    const auto event = board->eventWithRuntimeId(ed.fEventRuntimeId);
    if(!event) return;
    event->fillEventDataActions(ed);
}

#ifndef AGORA_BUILD_PLACE_H
#define AGORA_BUILD_PLACE_H

#include <vector>
#include "buildings/eagorabase.h"
#include "engine/ecityid.h"
#include "engine/stamps/estamptool.h"

class eTile;
class GameBoard;

bool agoraRoadTile(eTile* const t);

std::vector<eTile*> agoraBuildPlaceBR(GameBoard* board, bool editorMode,
                                      eTile* const tile,
                                      eCityId cid, ePlayerId pid);
std::vector<eTile*> agoraBuildPlaceTL(GameBoard* board, bool editorMode,
                                      eTile* const tile,
                                      eCityId cid, ePlayerId pid);
std::vector<eTile*> agoraBuildPlaceBL(GameBoard* board, bool editorMode,
                                      eTile* const tile,
                                      eCityId cid, ePlayerId pid);
std::vector<eTile*> agoraBuildPlaceTR(GameBoard* board, bool editorMode,
                                      eTile* const tile,
                                      eCityId cid, ePlayerId pid);

std::vector<eTile*> agoraBuildPlaceIter(GameBoard* board, bool editorMode,
                                        eTile* const tile, bool grand,
                                        eAgoraOrientation& bt,
                                        eCityId cid, ePlayerId pid);

std::vector<eTile*> stampAgoraBuildPlace(GameBoard* board, bool editorMode,
                                         const eStampBuildCommand& cmd,
                                         int pressedTX, int pressedTY,
                                         eAgoraOrientation& bt,
                                         eCityId cid, ePlayerId pid);

#endif // AGORA_BUILD_PLACE_H

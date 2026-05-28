#include "eattackingcityeventvalue.h"

#include "engine/e-worldcity.h"
#include "fileIO/esavearchive.h"

void eAttackingCityEventValue::serialize(eSaveArchive& ar, GameBoard* board) {
    ar.worldCityField("attackingCity", board, mAttackingCity);
}

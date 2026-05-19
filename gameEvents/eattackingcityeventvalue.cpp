#include "eattackingcityeventvalue.h"

#include "engine/e-worldcity.h"
#include "fileIO/esavearchive.h"

void eAttackingCityEventValue::serialize(eSaveArchive& ar, eGameBoard* board) {
    ar.worldCityField("attackingCity", board, mAttackingCity);
}

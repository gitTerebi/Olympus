#include "eattackingcityeventvalue.h"

#include "engine/world-city.h"
#include "fileIO/save-archive.h"

void eAttackingCityEventValue::serialize(SaveArchive& ar, GameBoard* board) {
    ar.worldCityField("attackingCity", board, mAttackingCity);
}

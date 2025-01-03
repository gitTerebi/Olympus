#include "ecityid.h"

bool eTeamIdHelpers::isEnemy(const eTeamId t1, const eTeamId t2) {
    if(t1 == eTeamId::neutralAggresive ||
       t2 == eTeamId::neutralAggresive) return true;
    if(t1 == eTeamId::neutralFriendly ||
       t2 == eTeamId::neutralFriendly) return false;
    return t1 != t2;
}

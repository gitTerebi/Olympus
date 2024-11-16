#ifndef ECITYID_H
#define ECITYID_H

enum class eCityId {
    neutralAggresive = -2,
    neutralFriendly = -1,
    city0 = 0,
    city1 = 1,
    city2 = 2,
    city3 = 3,
    city4 = 4,
    city5 = 5,
    city6 = 6,
    city7 = 7,
    city8 = 8,
    city9 = 9
};

enum class ePlayerId {
    neutralAggresive = -2,
    neutralFriendly = -1,
    player0 = 0,
    player1 = 1,
    player2 = 2,
    player3 = 3,
    player4 = 4,
    player5 = 5,
    player6 = 6,
    player7 = 7,
    player8 = 8,
    player9 = 9
};

enum class eTeamId {
    neutralAggresive = -2,
    neutralFriendly = -1,
    team0 = 0,
    team1 = 1,
    team2 = 2,
    team3 = 3,
    team4 = 4,
    team5 = 5,
    team6 = 6,
    team7 = 7,
    team8 = 8,
    team9 = 9
};

namespace eTeamIdHelpers {
    bool isEnemy(const eTeamId t1, const eTeamId t2);
}

#endif // ECITYID_H

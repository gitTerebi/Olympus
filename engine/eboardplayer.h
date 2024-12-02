#ifndef EBOARDPLAYER_H
#define EBOARDPLAYER_H

#include "ecityid.h"

#include "edate.h"
#include "edifficulty.h"

class eGameBoard;

class eBoardPlayer {
public:
    eBoardPlayer(eGameBoard& board);

    ePlayerId id() const { return mId; }
    void setId(const ePlayerId id) { mId = id; }

    void nextMonth();

    int drachmas() const { return mDrachmas; }
    void setDrachmas(const int d) { mDrachmas = d; }
    void incDrachmas(const int by);

    eDifficulty difficulty() const { return mDifficulty; }

    const eDate& inDebtSince() const { return mInDebtSince; }
private:
    bool isPerson() const;

    eGameBoard& mBoard;

    ePlayerId mId;

    eDifficulty mDifficulty{eDifficulty::beginner};

    int mDrachmas = 2500;
    eDate mInDebtSince;
};

#endif // EBOARDPLAYER_H

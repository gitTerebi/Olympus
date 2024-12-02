#ifndef EZEUSSANCTUARY_H
#define EZEUSSANCTUARY_H

#include "esanctuary.h"

class eZeusSanctuary: public eSanctuary  {
public:
    eZeusSanctuary(const int sw, const int sh,
                   eGameBoard& board, const eCityId cid);
};

class eAphroditeSanctuary: public eSanctuary  {
public:
    eAphroditeSanctuary(const int sw, const int sh,
                   eGameBoard& board, const eCityId cid);
};

class eApolloSanctuary: public eSanctuary  {
public:
    eApolloSanctuary(const int sw, const int sh,
                    eGameBoard& board, const eCityId cid);
};

class eAtlasSanctuary: public eSanctuary  {
public:
    eAtlasSanctuary(const int sw, const int sh,
                    eGameBoard& board, const eCityId cid);
};

class eAthenaSanctuary: public eSanctuary  {
public:
    eAthenaSanctuary(const int sw, const int sh,
                     eGameBoard& board, const eCityId cid);
};

class eDemeterSanctuary: public eSanctuary  {
public:
    eDemeterSanctuary(const int sw, const int sh,
                      eGameBoard& board, const eCityId cid);
};

class eDionysusSanctuary: public eSanctuary  {
public:
    eDionysusSanctuary(const int sw, const int sh,
                       eGameBoard& board, const eCityId cid);
};

class eHadesSanctuary: public eSanctuary  {
public:
    eHadesSanctuary(const int sw, const int sh,
                    eGameBoard& board, const eCityId cid);
};

class eHeraSanctuary: public eSanctuary  {
public:
    eHeraSanctuary(const int sw, const int sh,
                   eGameBoard& board, const eCityId cid);
};

class eHermesSanctuary: public eSanctuary  {
public:
    eHermesSanctuary(const int sw, const int sh,
                     eGameBoard& board, const eCityId cid);
};

class ePoseidonSanctuary: public eSanctuary  {
public:
    ePoseidonSanctuary(const int sw, const int sh,
                      eGameBoard& board, const eCityId cid);
};

#endif // EZEUSSANCTUARY_H

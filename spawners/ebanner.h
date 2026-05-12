#ifndef EBANNER_H
#define EBANNER_H

#include "pointers/estdpointer.h"

class eGameBoard;
class eReadStream;
class eWriteStream;
class eTile;
class eSaveArchive;
class eJsonArchive;
enum class eCityId;

enum class eBannerTypeS {
    boar,
    deer,
    landInvasion,
    entryPoint,
    exitPoint,
    monsterPoint,
    disasterPoint,
    wolf,
    seaInvasion,
    disembarkPoint,
    none,
    riverEntryPoint,
    riverExitPoint,
    landSlidePoint
};

class eBanner {
public:
    eBanner(const eBannerTypeS type,
            const int id,
            eTile* const tile,
            eGameBoard& board);
    virtual ~eBanner();

    eTile* tile() const { return mTile; }
    eCityId cityId() const;
    eGameBoard& board() { return mBoard; }

    int id() const { return mId; }

    void setIOID(const int id) { mIOID = id; }
    int ioID() const { return mIOID; }

    eBannerTypeS type() const { return mType; }

    static bool sBuildable(const eBannerTypeS type);
    bool buildable() const;

    virtual void read(eReadStream& src);;
    virtual void write(eWriteStream& dst) const;;
    virtual void serializeJson(eJsonArchive& ar);

    static stdsptr<eBanner> sCreate(const int id, eTile* const tile,
                                    eGameBoard& board,
                                    const eBannerTypeS type);
private:
    void serialize(eSaveArchive& ar);
    const eBannerTypeS mType;
    const int mId;
    eTile* mTile;
    eGameBoard& mBoard;
    int mIOID = -1;
};

#endif // EBANNER_H

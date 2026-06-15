#ifndef EBANNER_H
#define EBANNER_H

class GameBoard;
class eTile;
class eSaveArchive;
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
            GameBoard& board);
    virtual ~eBanner();

    eTile* tile() const { return mTile; }
    eCityId cityId() const;
    GameBoard& board();

    int id() const { return mId; }

    void setIOID(const int id) { mIOID = id; }
    int ioID() const { return mIOID; }

    eBannerTypeS type() const { return mType; }

    static bool sBuildable(const eBannerTypeS type);
    bool buildable() const;

    virtual void serialize(eSaveArchive& ar);

    static eBanner* sCreate(const int id, eTile* const tile,
                            GameBoard& board,
                            const eBannerTypeS type);
private:
    const eBannerTypeS mType;
    const int mId;
    eTile* const mTile;
    GameBoard& mBoard;
    int mIOID = -1;
};

#endif // EBANNER_H

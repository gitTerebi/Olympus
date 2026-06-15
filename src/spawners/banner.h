#ifndef BANNER_H
#define BANNER_H

class GameBoard;
class eTile;
class SaveArchive;
enum class eCityId;

enum class BannerTypeS {
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

class Banner {
public:
    Banner(const BannerTypeS type,
            const int id,
            eTile* const tile,
            GameBoard& board);
    virtual ~Banner();

    eTile* tile() const { return mTile; }
    eCityId cityId() const;
    GameBoard& board();

    int id() const { return mId; }

    void setIOID(const int id) { mIOID = id; }
    int ioID() const { return mIOID; }

    BannerTypeS type() const { return mType; }

    static bool sBuildable(const BannerTypeS type);
    bool buildable() const;

    virtual void serialize(SaveArchive& ar);

    static Banner* sCreate(const int id, eTile* const tile,
                            GameBoard& board,
                            const BannerTypeS type);
private:
    const BannerTypeS mType;
    const int mId;
    eTile* const mTile;
    GameBoard& mBoard;
    int mIOID = -1;
};

#endif // BANNER_H

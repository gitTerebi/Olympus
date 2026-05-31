#ifndef EMISSILE_H
#define EMISSILE_H

#include <functional>

#include "pointers/estdselfref.h"
#include "vec2.h"
#include "engine/game-board.h"
#include "fileIO/esavearchive.h"

class eTile;
class eSaveArchive;
enum class eTileSize : int;

struct ePathPoint {
    double fX;
    double fY;
    double fHeight;

    void serialize(eSaveArchive& ar) {
        ar.field("x", fX);
        ar.field("y", fY);
        ar.field("height", fHeight);
    }
};

class eMissilePath {
public:
    eMissilePath(const std::vector<ePathPoint>& pts) :
        mPos(pts.empty() ? ePathPoint{0., 0., 0.} : pts[0]),
        mPts(pts) {
        progress(0.011);
    }

    bool finished() const {
        return mPtId >= (int)mPts.size();
    }

    void progress(const double by) {
        if(by < 0.01) return;
        if(finished()) return;
        const auto& dst = mPts[mPtId];
        vec2d line{dst.fX - mPos.fX, dst.fY - mPos.fY};
        const vec2d heightLine{mPos.fHeight - dst.fHeight,
                               mPos.fHeight - dst.fHeight};
        const vec2d angleLine = line + heightLine;
        mAngle = angleLine.angle();
        const double len = line.length();
        if(by > len) {
            mPos = dst;
            mPtId++;
            return progress(by - len);
        } else {
            const double inc = by/len;
            line *= inc;
            mPos.fX += line.x;
            mPos.fY += line.y;
            const double hline = dst.fHeight - mPos.fHeight;
            mPos.fHeight += inc*hline;
        }
    }

    const ePathPoint& pos() const { return mPos; }
    double angle() const { return mAngle; }
    double height() const { return mPos.fHeight; }

    void serialize(eSaveArchive& ar) {
        ar.field("angle", mAngle);
        ar.objectField("position", mPos);
        ar.field("pointId", mPtId);
        ar.arrayField("points", mPts, [](eSaveArchive& itemAr, ePathPoint& pt) {
            pt.serialize(itemAr);
        });
    }
private:
    double mAngle;
    ePathPoint mPos;
    int mPtId = 0;
    std::vector<ePathPoint> mPts;
};

class eGodAct;

enum class eMissileType {
    arrow,
    god,
    rock,
    spear,
    wave,
    lava,
    dust,
    racingHorse
};

class eMissile : public eStdSelfRef {
public:
    eMissile(GameBoard& board, const eMissileType type,
             const std::vector<ePathPoint>& path = {});
    ~eMissile();

    GameBoard& board() const;

    void incTime(const int by);

    void destroy();

    virtual std::shared_ptr<eTexture>
        getTexture(const eTileSize size) const {
        (void)size;
        return nullptr;
    }

    double x() const;
    double y() const;

    double globalX() const;
    double globalY() const;

    void setFinishAction(const stdsptr<eGodAct>& act);

    void setSpeed(const double s) { mSpeed = s; }
    double speed() const { return mSpeed; }

    double angle() const { return mPath.angle(); }
    double height() const { return mPath.height(); }

    int textureTime() const { return mTime/20; }
    int time() const { return mTime; }

    eMissileType type() const { return mType; }

    void serialize(eSaveArchive& ar);

    template <class T>
    static stdsptr<T> sCreate(GameBoard& brd,
                              const int tx0, const int ty0,
                              const double h0,
                              const int tx1, const int ty1,
                              const double h1,
                              const double dh);
    static stdsptr<eMissile> sCreate(GameBoard& brd,
                                     const eMissileType type);
protected:
    virtual void serializeFields(eSaveArchive& ar);
private:
    void changeTile(eTile* const t);

    const eMissileType mType;

    GameBoard& mBoard;
    eMissilePath mPath;

    int mTime = 0;
    double mSpeed = 1;

    stdsptr<eGodAct> mFinish;

    eTile* mTile = nullptr;
};

template<class T>
stdsptr<T> eMissile::sCreate(GameBoard& brd,
                             const int tx0, const int ty0,
                             const double h0,
                             const int tx1, const int ty1,
                             const double h1,
                             const double dh) {
    std::vector<ePathPoint> path;
    const auto t = brd.tile(tx0, ty0);
    if(!t) return nullptr;
    const double ca = t->altitude() + h0;
    const auto tt = brd.tile(tx1, ty1);
    if(!tt) return nullptr;
    const double cca = tt->altitude() + h1;
    path.push_back(ePathPoint{(double)tx0, (double)ty0, ca});
    path.push_back(ePathPoint{0.75*tx0 + 0.25*tx1,
                              0.75*ty0 + 0.25*ty1,
                              0.75*ca + 0.25*cca + dh*0.75});
    path.push_back(ePathPoint{0.5*tx0 + 0.5*tx1,
                              0.5*ty0 + 0.5*ty1,
                              0.5*ca + 0.5*cca + dh});
    path.push_back(ePathPoint{0.25*tx0 + 0.75*tx1,
                              0.25*ty0 + 0.75*ty1,
                              0.25*ca + 0.75*cca + dh*0.75});
    path.push_back(ePathPoint{(double)tx1, (double)ty1, cca});
    const auto m = e::make_shared<T>(brd, path);
    m->incTime(0);
    return m;
}

#endif // EMISSILE_H

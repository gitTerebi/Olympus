#include "epatrolbuildingbase.h"

#include "engine/egameboard.h"
#include "engine/eguidedmovepathtask.h"

ePatrolBuildingBase::ePatrolBuildingBase(
        eGameBoard& board,
        const eCharGenerator& charGen,
        const eActGenerator& actGen,
        const eBuildingType type,
        const int sw, const int sh,
        const int maxEmployees,
        const eCityId cid) :
    eEmployingBuilding(board, type, sw, sh, maxEmployees, cid),
    mCharGenerator(charGen),
    mActGenerator(actGen) {

}

stdsptr<eCharacterAction>
ePatrolBuildingBase::sDefaultActGenerator(
           eCharacter* const c,
           ePatrolBuildingBase* const b,
           const std::vector<eOrientation>& path,
           const stdsptr<eDirectionTimes>& dirTimes) {
    return e::make_shared<ePatrolAction>(c, b, path, dirTimes);
}

ePatrolBuildingBase::ePatrolBuildingBase(eGameBoard& board,
        const eCharGenerator& charGen,
        const eBuildingType type,
        const int sw, const int sh,
        const int maxEmployees,
        const eCityId cid) :
    ePatrolBuildingBase(board, charGen, sDefaultActGenerator,
                        type, sw, sh, maxEmployees, cid) {}

ePatrolBuildingBase::~ePatrolBuildingBase() {
    if(mChar) mChar->kill();
}

void ePatrolBuildingBase::timeChanged(const int by) {
    if(mSpawnPatrolers && enabled()) {
        if(!mChar) {
            mSpawnTime += by*effectiveness();
            if(mSpawnTime > mWaitTime) {
                mSpawnTime = 0;
                const bool r = updatePathIfNeeded();
                if(r) mSpawnOnPathSet = true;
                else spawn();
            }
        }
    }
    eEmployingBuilding::timeChanged(by);
}

using ePatrolGuides = std::vector<ePatrolGuide>;
ePatrolGuides ePatrolBuildingBase::reversePatrolGuides() const {
    auto guides = mPatrolGuides;
    std::reverse(guides.begin(), guides.end());
    return guides;
}

void ePatrolBuildingBase::setPatrolGuides(const ePatrolGuides &g) {
    mPatrolGuides = g;
}

void ePatrolBuildingBase::setBothDirections(const bool both) {
    mBothDirections = both;
}

bool ePatrolBuildingBase::spawn() {
    if(!mCharGenerator || !mActGenerator) return false;
    const auto chr = mCharGenerator();
    mChar = chr.get();
    if(!mChar) return false;
    chr->setBothCityIds(cityId());
    if(mPatrolGuides.empty()) {
        eTile* t = nullptr;
        const auto bt = type();
        if(bt == eBuildingType::commonAgora ||
           bt == eBuildingType::grandAgora) {
            t = centerTile();
        } else {
            const auto ts = surroundingRoad(false, true);
            if(!ts.empty()) {
                const int tss = ts.size();
                if(++mSpawnRoadId >= tss) mSpawnRoadId = 0;
                t = ts[mSpawnRoadId];
            }
        }
        if(!t) return false;
        chr->changeTile(t);
    } else {
        chr->changeTile(centerTile());
    }
    bool reverse = false;
    if(mBothDirections) {
        mLastDirection = !mLastDirection;
        if(mLastDirection) reverse = true;
    }
    const auto a = mActGenerator(chr.get(), this,
                                 reverse ? mReversePath : mPath,
                                 mDirTimes);
    chr->setAction(a);
    return true;
}

void ePatrolBuildingBase::setSpawnPatrolers(const bool s) {
    mSpawnPatrolers = s;
}

bool ePatrolBuildingBase::spawnsPatrolers() const {
    return mCharGenerator && mActGenerator;
}

void ePatrolBuildingBase::read(eReadStream& src) {
    eEmployingBuilding::read(src);

    src >> mBothDirections;
    src >> mLastDirection;

    src >> mSpawnPatrolers;
    src >> mSpawnTime;
    src >> mSpawnRoadId;

    mDirTimes = src.readDirectionTimes(getBoard());

    src.readCharacter(&getBoard(), [this](eCharacter* const c) {
        mChar = c;
    });

    int n;
    src >> n;
    for(int i = 0; i < n; i++) {
        ePatrolGuide pg;
        src >> pg.fX;
        src >> pg.fY;
        mPatrolGuides.push_back(pg);
    }
}

void ePatrolBuildingBase::write(eWriteStream& dst) const {
    eEmployingBuilding::write(dst);

    dst << mBothDirections;
    dst << mLastDirection;

    dst << mSpawnPatrolers;
    dst << mSpawnTime;
    dst << mSpawnRoadId;

    dst.writeDirectionTimes(mDirTimes.get());

    dst.writeCharacter(mChar);

    dst << mPatrolGuides.size();
    for(const auto& pg : mPatrolGuides) {
        dst << pg.fX;
        dst << pg.fY;
    }
}

bool ePatrolBuildingBase::updatePathIfNeeded() {
    if(mPatrolGuides.empty()) {
        mPath.clear();
        mReversePath.clear();
        return false;
    }
    auto& board = getBoard();
    const auto cid = cityId();
    const auto c = board.boardCityWithId(cid);
    const int s = c->roadState();
    if(s == mPathState) return false;
    mPathState = s;
    return updatePath();
}

bool ePatrolBuildingBase::updatePath(const eAction& finish) {
    if(mPatrolGuides.empty()) {
        mPath.clear();
        mReversePath.clear();
        return false;
    }
    auto& board = getBoard();
    auto& tp = board.threadPool();
    const auto task = new eGuidedMovePathTask(this, finish);
    tp.queueTask(task);
    return true;
}

void ePatrolBuildingBase::setPath(const std::vector<eOrientation> &path,
                                  const std::vector<eOrientation> &reversePath) {
    mPath = path;
    mReversePath = reversePath;
    if(mSpawnOnPathSet) {
        mSpawnOnPathSet = false;
        spawn();
    }
}

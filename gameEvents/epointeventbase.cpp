#include "epointeventbase.h"

#include "engine/egameboard.h"

ePointEventBase::ePointEventBase(
        const eBannerTypeS btype,
        const eCityId cid,
        eGameBoard& board) :
    mBType(btype), mCid(cid), mBoard(board) {}

void ePointEventBase::write(eWriteStream &dst) const {
    dst << mPointId;
    dst << mMinPointId;
    dst << mMaxPointId;
}

void ePointEventBase::read(eReadStream &src) {
    src >> mPointId;
    src >> mMinPointId;
    src >> mMaxPointId;
}

void ePointEventBase::choosePointId() {
    if(mMinPointId >= mMaxPointId) {
        mPointId = mMinPointId;
        return;
    }
    std::vector<int> options;
    for(int i = mMinPointId; i <= mMaxPointId; i++) {
        options.push_back(i);
    }
    std::random_shuffle(options.begin(), options.end());
    for(const int i : options) {
        switch(mBType) {
        case eBannerTypeS::monsterPoint: {
            const auto tile = mBoard.monsterTile(mCid, i);
            if(tile) {
                mPointId = i;
                return;
            }
        } break;
        case eBannerTypeS::disasterPoint: {
            const auto tile = mBoard.disasterTile(mCid, i);
            if(tile) {
                mPointId = i;
                return;
            }
        } break;
        case eBannerTypeS::landInvasion:
        case eBannerTypeS::seaInvasion: {
            const auto tile = mBoard.invasionTile(mCid, i);
            if(tile) {
                mPointId = i;
                return;
            }
        } break;
        default:
            assert(false);
        }
    }
    mPointId = mMinPointId;
}

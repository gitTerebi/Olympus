#include "epointeventvalue.h"

#include "engine/e-game-board.h"
#include "fileIO/esavearchive.h"

ePointEventValue::ePointEventValue(
    const eBannerTypeS btype,
    const eCityId cid,
    eGameBoard& board,
    const eValidator &v) :
    mBType(btype), mCid(cid),
    mBoard(board), mValidator(v) {}

void ePointEventValue::write(eWriteStream &dst) const {
    eSaveArchive ar(dst);
    ar.field("mPointId", const_cast<int&>(mPointId));
    ar.field("mMinPointId", const_cast<int&>(mMinPointId));
    ar.field("mMaxPointId", const_cast<int&>(mMaxPointId));
}

void ePointEventValue::read(eReadStream &src) {
    eSaveArchive ar(src);
    ar.field("mPointId", mPointId);
    ar.field("mMinPointId", mMinPointId);
    ar.field("mMaxPointId", mMaxPointId);
}

void ePointEventValue::choosePointId() {
    std::vector<int> options;
    for(int i = mMinPointId; i <= mMaxPointId; i++) {
        options.push_back(i);
    }
    std::random_shuffle(options.begin(), options.end());
    for(const int i : options) {
        switch(mBType) {
        case eBannerTypeS::monsterPoint: {
            const auto tile = mBoard.monsterTile(mCid, i);
            if(tile && (!mValidator || mValidator(tile))) {
                mPointId = i;
                return;
            }
        } break;
        case eBannerTypeS::disasterPoint: {
            const auto tile = mBoard.disasterTile(mCid, i);
            if(tile && (!mValidator || mValidator(tile))) {
                mPointId = i;
                return;
            }
        } break;
        case eBannerTypeS::landSlidePoint: {
            const auto tile = mBoard.landSlideTile(mCid, i);
            if(tile && (!mValidator || mValidator(tile))) {
                mPointId = i;
                return;
            }
        } break;
        case eBannerTypeS::landInvasion:
        case eBannerTypeS::seaInvasion: {
            const auto tile = mBoard.invasionTile(mCid, i);
            if(tile && (!mValidator || mValidator(tile))) {
                mPointId = i;
                return;
            }
        } break;
        default:
            assert(false);
        }
    }
    mPointId = 0;
}

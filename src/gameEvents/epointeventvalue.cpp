#include "epointeventvalue.h"

#include "engine/game-board.h"
#include "fileIO/esavearchive.h"
#include "characters/soldier-banner.h"

ePointEventValue::ePointEventValue(
    const eBannerTypeS btype,
    const eCityId cid,
    GameBoard& board,
    const eValidator &v) :
    mBType(btype), mCid(cid),
    mBoard(board), mValidator(v) {}

void ePointEventValue::serialize(eSaveArchive& ar) {
    ar.field("pointId", mPointId, 1);
    ar.field("minPointId", mMinPointId, 1);
    ar.field("maxPointId", mMaxPointId, 1);
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

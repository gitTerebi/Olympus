#include "epointeventvalue.h"

#include "engine/egameboard.h"

ePointEventValue::ePointEventValue(
    const eBannerTypeS btype,
    const eCityId cid,
    eGameBoard& board,
    const eValidator &v) :
    mBType(btype), mCid(cid),
    mBoard(board), mValidator(v) {}

void ePointEventValue::write(eWriteStream &dst) const {
    dst << mPointId;
    dst << mMinPointId;
    dst << mMaxPointId;
}

void ePointEventValue::read(eReadStream &src) {
    src >> mPointId;
    src >> mMinPointId;
    src >> mMaxPointId;
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

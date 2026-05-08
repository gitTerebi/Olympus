#include "eruins.h"

#include "textures/egametextures.h"
#include "fileIO/esavearchive.h"

eRuins::eRuins(eGameBoard& board, const eCityId cid) :
    eBuilding(board, eBuildingType::ruins, 1, 1, cid) {

}

stdsptr<eTexture> eRuins::getTexture(const eTileSize size) const {
    const int sizeId = static_cast<int>(size);
    const auto& texs = eGameTextures::terrain();
    const auto& coll = texs[sizeId].fTinyStones;
    return coll.getTexture(seed() % coll.size());
}

struct eByteVecRef {
    std::vector<uint8_t>& fVec;
};

static eWriteStream& operator<<(eWriteStream& dst, const eByteVecRef& ref) {
    const int32_t sz = static_cast<int32_t>(ref.fVec.size());
    dst.write(&sz, sizeof(sz));
    if(sz > 0) dst.write(ref.fVec.data(), sz);
    return dst;
}

static eReadStream& operator>>(eReadStream& src, eByteVecRef& ref) {
    int32_t sz;
    src.read(&sz, sizeof(sz));
    ref.fVec.clear();
    if(sz > 0) {
        ref.fVec.resize(sz);
        src.read(ref.fVec.data(), sz);
    }
    return src;
}

static void byteVecField(eSaveArchive& ar, const char* const name,
                         std::vector<uint8_t>& v) {
    eByteVecRef ref{v};
    ar.field(name, ref);
}

void eRuins::serialize(eSaveArchive& ar) {
    ar.field("mWasType", mWasType);
    ar.field("mOriginX", mOriginX);
    ar.field("mOriginY", mOriginY);
    ar.field("mOriginW", mOriginW);
    ar.field("mOriginH", mOriginH);
    byteVecField(ar, "mSavedBuilding", mSavedBuilding);
    byteVecField(ar, "mSavedPier", mSavedPier);
    ar.field("mSavedPierRect", mSavedPierRect);
    byteVecField(ar, "mRestoreBundle", mRestoreBundle);
}

void eRuins::read(eReadStream& src) {
    eBuilding::read(src);
    eSaveArchive ar(src);
    serialize(ar);
}

void eRuins::write(eWriteStream& dst) const {
    eBuilding::write(dst);
    eSaveArchive ar(dst);
    const_cast<eRuins*>(this)->serialize(ar);
}

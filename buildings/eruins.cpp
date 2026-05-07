#include "eruins.h"

#include "textures/egametextures.h"
#include "fileIO/esavearchive.h"
#include "fileIO/efileformat.h"

eRuins::eRuins(eGameBoard& board, const eCityId cid) :
    eBuilding(board, eBuildingType::ruins, 1, 1, cid) {

}

stdsptr<eTexture> eRuins::getTexture(const eTileSize size) const {
    const int sizeId = static_cast<int>(size);
    const auto& texs = eGameTextures::terrain();
    const auto& coll = texs[sizeId].fTinyStones;
    return coll.getTexture(seed() % coll.size());
}

static void writeByteVec(eWriteStream& dst, const std::vector<uint8_t>& v) {
    const int32_t sz = static_cast<int32_t>(v.size());
    dst << sz;
    if(sz > 0) dst.write(v.data(), sz);
}

static void readByteVec(eReadStream& src, std::vector<uint8_t>& v) {
    int32_t sz;
    src >> sz;
    if(sz > 0) {
        v.resize(sz);
        src.read(v.data(), sz);
    }
}

static void byteVec(eSaveArchive& ar, std::vector<uint8_t>& v) {
    if(ar.reading()) {
        readByteVec(ar.readStream(), v);
    } else {
        writeByteVec(ar.writeStream(), v);
    }
}

void eRuins::serialize(eSaveArchive& ar) {
    ar.value(mWasType);
    if(ar.versionAtLeast(eFileFormat::ruinsOrigin)) {
        ar.value(mOriginX);
        ar.value(mOriginY);
        ar.value(mOriginW);
        ar.value(mOriginH);
    }
    if(ar.versionAtLeast(eFileFormat::ruinsSavedBuilding)) {
        byteVec(ar, mSavedBuilding);
        byteVec(ar, mSavedPier);
        ar.value(mSavedPierRect);
    }
    if(ar.versionAtLeast(eFileFormat::ruinsRestoreBundle)) {
        byteVec(ar, mRestoreBundle);
    }
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

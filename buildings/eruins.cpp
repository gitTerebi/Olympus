#include "eruins.h"

#include "textures/egametextures.h"
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

void eRuins::read(eReadStream& src) {
    eBuilding::read(src);
    src >> mWasType;
    if(src.formatVersion() >= eFileFormat::ruinsOrigin) {
        src >> mOriginX >> mOriginY >> mOriginW >> mOriginH;
    }
    if(src.formatVersion() >= eFileFormat::ruinsSavedBuilding) {
        readByteVec(src, mSavedBuilding);
        readByteVec(src, mSavedPier);
        src >> mSavedPierRect;
    }
}

void eRuins::write(eWriteStream& dst) const {
    eBuilding::write(dst);
    dst << mWasType;
    dst << mOriginX << mOriginY << mOriginW << mOriginH;
    writeByteVec(dst, mSavedBuilding);
    writeByteVec(dst, mSavedPier);
    dst << mSavedPierRect;
}

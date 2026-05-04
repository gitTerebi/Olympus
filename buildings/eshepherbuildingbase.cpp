#include "eshepherbuildingbase.h"

#include "textures/egametextures.h"
#include "characters/actions/eshepherdaction.h"
#include "fileIO/efileformat.h"

eShepherBuildingBase::eShepherBuildingBase(
        eGameBoard& board,
        const eBaseTex baseTex,
        const double overlayX,
        const double overlayY,
        const eOverlays overlays,
        const eCharGenerator& charGen,
        const eBuildingType type,
        const eResourceType resType,
        const eCharacterType animalType,
        const int sw, const int sh,
        const int maxEmployees,
        const eCityId cid) :
    eResourceBuildingBase(board, type, sw, sh,
                          maxEmployees, resType, cid),
    mCharGenerator(charGen),
    mTextures(eGameTextures::buildings()),
    mBaseTex(baseTex), mOverlays(overlays),
    mOverlayX(overlayX), mOverlayY(overlayY),
    mAnimalType(animalType) {

}

eShepherBuildingBase::~eShepherBuildingBase() {
    if(mShepherd) mShepherd->kill();
}

std::shared_ptr<eTexture> eShepherBuildingBase::getTexture(const eTileSize size) const {
    const int sizeId = static_cast<int>(size);
    return mTextures[sizeId].*mBaseTex;
}

std::vector<eOverlay> eShepherBuildingBase::getOverlays(
        const eTileSize size) const {
    const int sizeId = static_cast<int>(size);
    const auto& coll = mTextures[sizeId].*mOverlays;
    const int texId = textureTime() % coll.size();
    eOverlay o;
    o.fTex = coll.getTexture(texId);
    o.fX = mOverlayX;
    o.fY = mOverlayY;
    return std::vector<eOverlay>({o});
}

void eShepherBuildingBase::timeChanged(const int by) {
    eResourceBuildingBase::timeChanged(by);
    if(!mShepherd) {
        mSpawnTime += by*effectiveness();
        const int period = eNumbers::sShepherdGoatherdWaitTime;
        if(mSpawnTime > period) {
            mSpawnTime = 0;
            spawn();
        }
    }
}

void eShepherBuildingBase::nextMonth() {
    mRingIdx = (mRingIdx + 1) % 12;
    mProducedThisYear -= mMonthlyProduced[mRingIdx];
    mMonthlyProduced[mRingIdx] = 0;
    if(mProducedThisYear < 0) mProducedThisYear = 0;
}

void eShepherBuildingBase::shepherdDelivered(const eResourceType type, const int count) {
    const int c = addProduced(type, count);
    mProducedThisYear += c;
    mMonthlyProduced[mRingIdx] += c;
}

void eShepherBuildingBase::read(eReadStream& src) {
    eResourceBuildingBase::read(src);
    src.readCharacter(&getBoard(), [this](eCharacter* const c) {
        mShepherd = static_cast<eResourceCollectorBase*>(c);
    });
    src >> mSpawnTime;
    if(src.formatVersion() >= eFileFormat::yearlyProduction) {
        src >> mProducedThisYear;
        for(int i = 0; i < 12; i++) src >> mMonthlyProduced[i];
        src >> mRingIdx;
    }
}

void eShepherBuildingBase::write(eWriteStream& dst) const {
    eResourceBuildingBase::write(dst);
    dst.writeCharacter(mShepherd.get());
    dst << mSpawnTime;
    dst << mProducedThisYear;
    for(int i = 0; i < 12; i++) dst << mMonthlyProduced[i];
    dst << mRingIdx;
}

bool eShepherBuildingBase::spawn() {
    if(resource() >= maxResource()) return false;
    const auto t = centerTile();
    const auto s = mCharGenerator(getBoard());
    s->setBothCityIds(cityId());
    mShepherd = s.get();
    s->changeTile(t);
    const auto a = e::make_shared<eShepherdAction>(
                       this, s.get(),
                       mAnimalType);
    s->setAction(a);
    return true;
}

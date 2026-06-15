#include "eshepherbuildingbase.h"

#include "textures/game-textures.h"
#include "characters/actions/shepherd-action.h"
#include "fileIO/save-archive.h"

eShepherBuildingBase::eShepherBuildingBase(
        GameBoard& board,
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
    mTextures(GameTextures::buildings()),
    mBaseTex(baseTex), mOverlays(overlays),
    mOverlayX(overlayX), mOverlayY(overlayY),
    mAnimalType(animalType) {

}

eShepherBuildingBase::~eShepherBuildingBase() {
    if(mShepherd) mShepherd->kill();
}

std::shared_ptr<Texture> eShepherBuildingBase::getTexture(const eTileSize size) const {
    const int sizeId = static_cast<int>(size);
    return mTextures[sizeId].*mBaseTex;
}

std::vector<Overlay> eShepherBuildingBase::getOverlays(
        const eTileSize size) const {
    const int sizeId = static_cast<int>(size);
    const auto& coll = mTextures[sizeId].*mOverlays;
    const int texId = textureTime() % coll.size();
    Overlay o;
    o.fTex = coll.getTexture(texId);
    o.fX = mOverlayX;
    o.fY = mOverlayY;
    return std::vector<Overlay>({o});
}

void eShepherBuildingBase::timeChanged(const int by) {
    eResourceBuildingBase::timeChanged(by);
    if(!mShepherd) {
        mSpawnTime += by*effectiveness();
        const int period = Numbers::sShepherdGoatherdWaitTime;
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

void eShepherBuildingBase::serializeFields(SaveArchive& ar) {
    eResourceBuildingBase::serializeFields(ar);
    ar.characterField("shepherd", &getBoard(), mShepherd);
    ar.field("spawnTime", mSpawnTime);
    ar.field("producedThisYear", mProducedThisYear);
    for(int i = 0; i < 12; i++) {
        ar.field(("monthlyProduced." + std::to_string(i)).c_str(),
                 mMonthlyProduced[i]);
    }
    ar.field("ringIdx", mRingIdx);
}


bool eShepherBuildingBase::spawn() {
    if(resource() >= maxResource()) return false;
    const auto t = centerTile();
    const auto s = mCharGenerator(getBoard());
    s->setBothCityIds(cityId());
    mShepherd = s.get();
    s->changeTile(t);
    const auto a = e::make_shared<ShepherdAction>(
                       this, s.get(),
                       mAnimalType);
    s->setAction(a);
    return true;
}

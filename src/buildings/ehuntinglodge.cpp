#include "ehuntinglodge.h"

#include "characters/ehunter.h"
#include "textures/game-textures.h"
#include "characters/actions/ehuntaction.h"
#include "numbers.h"
#include "fileIO/save-archive.h"

#include <algorithm>

eHuntingLodge::eHuntingLodge(GameBoard& board,
                             const eCityId cid) :
    eResourceCollectBuildingBase(board, eBuildingType::huntingLodge,
                          2, 2, 8, eResourceType::meat, cid) {
    GameTextures::loadHuntingLodge();
}

eHuntingLodge::~eHuntingLodge() {
    if(mHunter) mHunter->kill();
}

std::shared_ptr<Texture> eHuntingLodge::getTexture(const eTileSize size) const {
    const int sizeId = static_cast<int>(size);
    return GameTextures::buildings()[sizeId].fHuntingLodge;
}

std::vector<Overlay> eHuntingLodge::getOverlays(const eTileSize size) const {
    const int sizeId = static_cast<int>(size);
    const auto& texs = GameTextures::buildings()[sizeId];
    const auto& coll = texs.fHuntingLodgeOverlay;
    const int texId = textureTime() % coll.size();
    Overlay o;
    o.fTex = coll.getTexture(texId);
    o.fX = -1.95;
    o.fY = -2.4;
    if(resource() > 0) {
        Overlay meat;
        const int res = std::clamp(resource() - 1, 0, 4);
        meat.fTex = texs.fWaitingMeat.getTexture(res);
        meat.fX = -0.5;
        meat.fY = -2;

        return std::vector<Overlay>({o, meat});
    }
    return std::vector<Overlay>({o});
}

void eHuntingLodge::timeChanged(const int by) {
    eResourceCollectBuildingBase::timeChanged(by);
    if(!mHunter) {
        const double eff = effectiveness();
        mSpawnTime += by*eff;
        const int wait = Numbers::sHuntingLodgeWaitPeriod;
        if(mSpawnTime > wait) {
            mSpawnTime -= wait;
            spawn();
        }
    }
}

void eHuntingLodge::serializeFields(SaveArchive& ar) {
    eResourceCollectBuildingBase::serializeFields(ar);
    ar.field("spawnTime", mSpawnTime);
    ar.characterField("hunter", &getBoard(), mHunter);
}

void eHuntingLodge::hunterDelivered(const eResourceType type, const int count) {
    const int c = addProduced(type, count);
    trackProduced(c);
}

bool eHuntingLodge::spawn() {
    const auto t = centerTile();
    const auto h = e::make_shared<eHunter>(getBoard());
    mHunter = h.get();
    h->setBothCityIds(cityId());
    h->setAtlantean(atlantean());
    h->changeTile(t);
    const auto a = e::make_shared<eHuntAction>(this, h.get());
    h->setAction(a);
    return true;
}

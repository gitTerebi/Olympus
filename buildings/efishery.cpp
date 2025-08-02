#include "efishery.h"

#include "characters/efishingboat.h"
#include "characters/actions/ecollectresourceaction.h"
#include "textures/egametextures.h"
#include "engine/egameboard.h"
#include "enumbers.h"

eFishery::eFishery(eGameBoard& board,
                   const eDiagonalOrientation o,
                   const eCityId cid) :
    eResourceCollectBuildingBase(board, eBuildingType::fishery,
                                 2, 2, 10, eResourceType::fish,
                                 cid),
    mO(o) {
    eGameTextures::loadFishery();
}

eFishery::~eFishery() {
    if(mBoat) mBoat->kill();
}

void eFishery::timeChanged(const int by) {
    if(enabled()) {
        mStateCount += by;
        if(!mBoat && mState != eFisheryState::buildingBoat) {
            mState = eFisheryState::buildingBoat;
            mStateCount = 0;
        }
        switch(mState) {
        case eFisheryState::buildingBoat: {
            if(mStateCount > eNumbers::sFisheryBoatBuildTime) {
                mStateCount = 0;
                mState = eFisheryState::waiting;
                spawnBoat();
            }
        } break;
        case eFisheryState::unpacking: {
            if(mStateCount > eNumbers::sFisheryUnpackTime) {
                mStateCount = 0;
                mState = eFisheryState::waiting;
                addProduced(eResourceType::fish, 3);
                updateDisabled();
            }
        } break;
        default: break;
        }
    }
    eResourceBuildingBase::timeChanged(by);
}

std::shared_ptr<eTexture> eFishery::getTexture(const eTileSize size) const {
    const int sizeId = static_cast<int>(size);
    const auto& blds = eGameTextures::buildings();
    const auto& coll = blds[sizeId].fFishery;
    auto& board = getBoard();
    const auto dir = board.direction();
    const auto o = sRotated(mO, dir);
    int id = 3;
    switch(o) {
    case eDiagonalOrientation::topRight:
        id = 0;
        break;
    case eDiagonalOrientation::bottomRight:
        id = 1;
        break;
    case eDiagonalOrientation::bottomLeft:
        id = 2;
        break;
    case eDiagonalOrientation::topLeft:
        id = 3;
        break;
    default:
        id = 0;
        break;
    }

    return coll.getTexture(id);
}

std::vector<eOverlay> eFishery::getOverlays(const eTileSize size) const {
    const int sizeId = static_cast<int>(size);
    const auto& blds = eGameTextures::buildings()[sizeId];
    auto& board = getBoard();
    const auto dir = board.direction();
    const auto oo = sRotated(mO, dir);
    switch(mState) {
    case eFisheryState::buildingBoat: {
        eOverlay o;
        const eTextureCollection* coll;
        switch(oo) {
        case eDiagonalOrientation::topRight:
            coll = &blds.fFisheryBoatBuildingH;
            o.fX = 0.2;
            o.fY = -2.25;
            break;
        case eDiagonalOrientation::bottomLeft:
            coll = &blds.fFisheryBoatBuildingH;
            o.fX = -0.3;
            o.fY = -2.38;
            break;
        default:
        case eDiagonalOrientation::topLeft:
            coll = &blds.fFisheryBoatBuildingW;
            o.fX = 0.0;
            o.fY = -2.0;
            break;
        case eDiagonalOrientation::bottomRight:
            coll = &blds.fFisheryBoatBuildingW;
            o.fX = -0.25;
            o.fY = -1.9;
            break;
        }

        const int texId = textureTime() % coll->size();
        o.fTex = coll->getTexture(texId);
        return {o};
    } break;
    case eFisheryState::waiting: {
        eOverlay o;
        const eTextureCollection* coll;
        switch(oo) {
        case eDiagonalOrientation::topRight:
            coll = &blds.fFisheryOverlay[0];
            o.fX = 0.2;
            o.fY = -2.0;
            break;
        case eDiagonalOrientation::bottomLeft:
            coll = &blds.fFisheryOverlay[4];
            o.fX = -0.3;
            o.fY = -2.38;
            break;
        default:
        case eDiagonalOrientation::topLeft:
            coll = &blds.fFisheryOverlay[6];
            o.fX = 0.25;
            o.fY = -2.2;
            break;
        case eDiagonalOrientation::bottomRight:
            coll = &blds.fFisheryOverlay[2];
            o.fX = -0.25;
            o.fY = -1.9;
            break;
        }

        const int texId = textureTime() % coll->size();
        o.fTex = coll->getTexture(texId);
        return {o};
    } break;
    case eFisheryState::unpacking: {
        eOverlay o;
        const eTextureCollection* coll;
        switch(oo) {
        case eDiagonalOrientation::topRight:
            coll = &blds.fFisheryUnpackingOverlayTR;
            o.fX = -0.3;
            o.fY = -2.95;
            break;
        case eDiagonalOrientation::bottomLeft:
            coll = &blds.fFisheryUnpackingOverlayBL;
            o.fX = -0.35;
            o.fY = -1.75;
            break;
        default:
        case eDiagonalOrientation::topLeft:
            coll = &blds.fFisheryUnpackingOverlayTL;
            o.fX = -0.75;
            o.fY = -2.3;
            break;
        case eDiagonalOrientation::bottomRight:
            coll = &blds.fFisheryUnpackingOverlayBR;
            o.fX = 0.20;
            o.fY = -2.3;
            break;
        }

        const int texId = textureTime() % coll->size();
        o.fTex = coll->getTexture(texId);
        return {o};
    } break;
    default:
        break;
    }
    return {};
}

void eFishery::addRaw() {
    mState = eFisheryState::unpacking;
    mStateCount = 0;
    if(mBoat) {
        eDiagonalOrientation o;
        switch(mO) {
        case eDiagonalOrientation::bottomLeft:
        case eDiagonalOrientation::topRight:
            o = eDiagonalOrientation::topLeft;
            break;
        default:
        case eDiagonalOrientation::bottomRight:
        case eDiagonalOrientation::topLeft:
            o = eDiagonalOrientation::topRight;
            break;
        }
        mBoat->setOrientation(sConvert(o));
    }
}

int eFishery::take(const eResourceType type, const int count) {
    const int r = eResourceCollectBuildingBase::take(type, count);
    updateDisabled();
    return r;
}

void eFishery::spawnBoat() {
    if(mBoat) return;
    const auto b = e::make_shared<eFishingBoat>(getBoard());
    b->setBothCityIds(cityId());
    mBoat = b.get();
    eTile* t;
    const auto ct = centerTile();
    switch(mO) {
    case eDiagonalOrientation::topRight: {
        const auto tr = ct->topRight<eTile>();
        if(!tr) return;
        t = tr->topRight<eTile>();
    } break;
    case eDiagonalOrientation::bottomLeft:
        t = ct->bottom<eTile>();
        break;
    case eDiagonalOrientation::topLeft:
        t = ct->topLeft<eTile>();
        break;
    default:
    case eDiagonalOrientation::bottomRight: {
        const auto r = ct->right<eTile>();
        if(!r) return;
        t = r->bottomRight<eTile>();
    } break;
    }
    if(!t) return;
    b->changeTile(t);

    const auto hasRes = eHasResourceObject::sCreate(
                            eHasResourceObjectType::fish);
    const auto a = e::make_shared<eCollectResourceAction>(
                       this, b.get(), hasRes);
    const auto w = eWalkableObject::sCreateDeepWater();
    a->setWalkable(w);
    a->setWaitTime(eNumbers::sFisheryUnpackTime);
    a->setFinishOnce(false);
    b->setAction(a);
}

void eFishery::updateDisabled() {
    const int s = eResourceBuildingBase::spaceLeft(eResourceType::fish);
    const bool d = s <= 0;
    if(mDisabled == d) return;
    mDisabled = d;
}

void eFishery::read(eReadStream& src) {
    eResourceCollectBuildingBase::read(src);

    src >> mDisabled;
    src >> mStateCount;
    src >> mState;
    src.readCharacter(&getBoard(), [this](eCharacter* const c) {
        mBoat = static_cast<eFishingBoat*>(c);
    });
}

void eFishery::write(eWriteStream& dst) const {
    eResourceCollectBuildingBase::write(dst);

    dst << mDisabled;
    dst << mStateCount;
    dst << mState;
    dst.writeCharacter(mBoat);
}

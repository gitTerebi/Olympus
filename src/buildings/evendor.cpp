#include "evendor.h"

#include "characters/echaracter.h"
#include "characters/ecarttransporter.h"
#include "textures/game-textures.h"
#include "fileIO/esavearchive.h"
#include "buildings/eagorabase.h"
#include "characters/actions/eactionwithcomeback.h"
#include "characters/actions/vendor-cart-action.h"
#include "engine/game-board.h"
#include "enumbers.h"

#include <algorithm>
#include <cstdio>

eVendor::eVendor(GameBoard& board,
                 const eResourceType resType,
                 const eProvide provType,
                 const eBaseTex& baseTex,
                 const double overlayX,
                 const double overlayY,
                 const eBaseTex& overlayTex,
                 const double overlayX2,
                 const double overlayY2,
                 const eOverlays& overlayTex2,
                 const eBuildingType type,
                 const int sw, const int sh,
                 const int maxEmployees,
                 const eCityId cid) :
    eEmployingBuilding(board, type, sw, sh, maxEmployees, cid),
    mResType(resType),
    mProvType(provType),
    mBaseTex(baseTex),
    mOverlayX(overlayX),
    mOverlayY(overlayY),
    mOverlayTex(overlayTex),
    mOverlayX2(overlayX2),
    mOverlayY2(overlayY2),
    mOverlayTex2(overlayTex2) {
    GameTextures::loadAgora();
}

eVendor::~eVendor() {
    if(mCart) mCart->kill();
}

std::shared_ptr<eTexture> eVendor::getTexture(const eTileSize size) const {
    const int sizeId = static_cast<int>(size);
    const auto& blds = GameTextures::buildings();
    return blds[sizeId].*mBaseTex;
}

std::vector<eOverlay> eVendor::getOverlays(const eTileSize size) const {
    if(mResource <= 0) return {};

    auto os = eEmployingBuilding::getOverlays(size);
    const int sizeId = static_cast<int>(size);
    const auto& texs = GameTextures::buildings();

    if(mOverlayTex) {
        eOverlay o;
        o.fTex = texs[sizeId].*mOverlayTex;
        o.fX = mOverlayX;
        o.fY = mOverlayY;
        os.push_back(o);
    }

    if(mOverlayTex2) {
        eOverlay o2;
        const auto& coll = texs[sizeId].*mOverlayTex2;
        o2.fTex = coll.getTexture(textureTime() % coll.size());
        o2.fX = mOverlayX2;
        o2.fY = mOverlayY2;
        os.push_back(o2);
    }

    return os;
}

void eVendor::erase() {
    deleteLater();
    const auto a = agora();
    if(!a) return;
    a->setBuilding(this, nullptr);
    a->fillSpaces();
}

int eVendor::add(const eResourceType type, const int count) {
    if(!static_cast<bool>(type & mResType)) return 0;

    const int r = std::clamp(count, 0, spaceLeft(type));
    mResource += r*mResMult;
    return r;
}

int eVendor::take(const eResourceType type, const int count) {
    (void)type;
    (void)count;
    return 0;
}

int eVendor::count(const eResourceType type) const {
    (void)type;
    return 0;
}

int eVendor::spaceLeft(const eResourceType type) const {
    if(type != mResType) return 0;
    return (mMaxResource - mResource)/mResMult;
}

eCartTransporter* eVendor::spawnVendorCart() {
    const auto t = centerTile();
    auto& board = getBoard();
    const auto c = e::make_shared<eCartTransporter>(board);
    c->setBothCityIds(cityId());
    c->setAtlantean(atlantean());
    c->changeTile(t);
    const auto a = e::make_shared<eVendorCartAction>(c.get(), this);
    c->setAction(a);
    c->setSupport(eCartActionTypeSupport::get);
    return c.get();
}

void eVendor::timeChanged(const int by) {
    if(enabled()) {
        if(!mCart) {
            mCart = spawnVendorCart();
            if(mCart && mAgora) {
                const auto st = mAgora->patrolStartTile();
                if(st) {
                    mCart->changeTile(st);
                    const auto a = dynamic_cast<eActionWithComeback*>(
                                       mCart->action());
                    if(a) a->setStartTile(st);
                }
            }
            switch(mResType) {
            case eResourceType::food:
                mCart->setType(eCartTransporterType::food);
                break;
            case eResourceType::fleece:
                mCart->setType(eCartTransporterType::fleece);
                break;
            case eResourceType::oliveOil:
                mCart->setType(eCartTransporterType::oil);
                break;
            case eResourceType::wine:
                mCart->setType(eCartTransporterType::wine);
                break;
            case eResourceType::armor:
                mCart->setType(eCartTransporterType::arms);
                break;
            case eResourceType::horse:
                mCart->setType(eCartTransporterType::horse);
                break;
            case eResourceType::chariot:
                mCart->setType(eCartTransporterType::chariot);
                break;
            default:
                break;
            }
        }
        if(mCart) {
            switch(mResType) {
            case eResourceType::food:
            case eResourceType::fleece:
            case eResourceType::oliveOil:
            case eResourceType::wine:
            case eResourceType::armor:
                mCart->setMaxDistance(eNumbers::sBasicVendorMaxResourceTakeDistance);
                break;
            case eResourceType::horse:
            case eResourceType::chariot:
                mCart->setMaxDistance(eNumbers::sHorseVendorMaxResourceTakeDistance);
                break;
            default:
                break;
            }
        }
    }
    eEmployingBuilding::timeChanged(by);
}

std::vector<eCartTask> eVendor::cartTasks() const {
    if(!mVendorEnabled) return {};
    const int space = spaceLeft(mResType);

    if(space > 0) {
        eCartTask task;
        task.fType = eCartActionType::get;
        task.fResource = mResType;
        task.fMaxCount = space;
        return {task};
    }

    return {};
}

void eVendor::setAgora(const stdsptr<eAgoraBase>& agora) {
    mAgora = agora;
}

int eVendor::agoraSpaceId() const {
    if(!mAgora) return -1;
    return mAgora->buildingId(this);
}

int eVendor::peddlerResource() const {
    return mResource;
}

int eVendor::takeForPeddler(const int t) {
    const int tt = std::clamp(t, 0, mResource);
    mResource -= tt;
    return tt;
}

void eVendor::serializeFields(eSaveArchive& ar) {
    eEmployingBuilding::serializeFields(ar);
    int agoraId = ar.writing() && mAgora ? mAgora->ioID() : -1;
    int agoraSpaceId = ar.writing() ? this->agoraSpaceId() : -1;
    ar.field("agoraId", agoraId, -1);
    ar.field("agoraSpaceId", agoraSpaceId, -1);
    if(ar.reading()) {
        const stdptr<eVendor> tptr(this);
        ar.addPostFunc([tptr, agoraId, agoraSpaceId]() {
            if(!tptr) return;
            if(agoraId < 0 || agoraSpaceId < 0) return;
            const auto a = tptr->getBoard().buildingWithIOID(agoraId);
            if(!a) {
                printf("vendor load: missing tagged agora aid=%d sid=%d\n",
                       agoraId, agoraSpaceId);
                return;
            }
            const auto agora = dynamic_cast<eAgoraBase*>(a);
            if(!agora) {
                printf("vendor load: bad tagged agora aid=%d sid=%d\n",
                       agoraId, agoraSpaceId);
                return;
            }
            const auto aa = agora->ref<eAgoraBase>();
            tptr->setAgora(aa);
            aa->setBuilding(agoraSpaceId, tptr->ref<eBuilding>());
        }, "eVendor::agora");
    }
    ar.field("resource", mResource);
    ar.field("vendorEnabled", mVendorEnabled);
    ar.characterField("cart", &getBoard(), mCart);
}

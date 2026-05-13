#include "ecarttransporter.h"

#include "textures/egametextures.h"

#include "characters/actions/efollowaction.h"
#include "engine/e-game-board.h"
#include "fileIO/esavearchive.h"
#include "fileIO/ejsonarchive.h"
#include "etrailer.h"
#include "eox.h"
#include "eporter.h"
#include "ehorse.h"
#include "echariot.h"

namespace {
template <typename T>
void killAndClear(stdptr<T>& c) {
    c.clear();
}

eCharacter* liveCharacter(eCharacter* const c) {
    if(!c) return nullptr;
    if(c->deleteScheduled()) return nullptr;
    return c;
}

void assignOx(stdptr<eOx>& ox, eCharacter* const c) {
    ox = dynamic_cast<eOx*>(liveCharacter(c));
}

void assignTrailer(stdptr<eTrailer>& trailer, eCharacter* const c) {
    trailer = dynamic_cast<eTrailer*>(liveCharacter(c));
}

int liveFollowerCount(const std::vector<stdptr<eCharacter>>& followers) {
    int result = 0;
    for(const auto& f : followers) {
        if(!f) break;
        result++;
    }
    return result;
}
}

eCartTransporter::eCartTransporter(eGameBoard& board) :
    eBasicPatroler(board, &eCharacterTextures::fTransporter,
                   eCharacterType::cartTransporter) {
    eGameTextures::loadTransporter();
    eGameTextures::loadCart();
    setHasSecondaryTexture(true);
}

eCartTransporter::~eCartTransporter() {
    setResourceValue(eResourceType::none, 0);
    killAndClear(mOx);
    killAndClear(mTrailer);
    for(const auto& f : mFollowers) {
        (void)f;
    }
}

eOverlay eCartTransporter::getSecondaryTexture(const eTileSize size) const {
    if(mType != eCartTransporterType::basic) {
        return eBasicPatroler::getSecondaryTexture(size);
    }
    const auto a = actionType();
    if(a == eCharacterActionType::none) {
        return eOverlay{0, 0, std::shared_ptr<eTexture>()};
    }
    const int id = static_cast<int>(size);
    const auto& texs = eGameTextures::characters()[id];
    const int oi = static_cast<int>(rotatedOrientation());

    int ci = 0;

    const auto rType = resType();
    const int rCount = resCount();

    switch(rType) {
    case eResourceType::urchin:
    case eResourceType::fish:
    case eResourceType::meat:
    case eResourceType::cheese:
    case eResourceType::carrots:
    case eResourceType::onions:
    case eResourceType::wheat:
    case eResourceType::oranges:

    case eResourceType::grapes:
    case eResourceType::olives:
        ci = std::clamp(rCount/4, 0, 2);
        break;
    case eResourceType::wine:
    case eResourceType::oliveOil:
    case eResourceType::fleece:
    case eResourceType::bronze:
    case eResourceType::orichalc:
    case eResourceType::armor:
        ci = std::clamp(rCount/4, 0, 1);
        break;
    default: break;
    }

    double xx;
    double yy;
    switch(rotatedOrientation()) {
    case eOrientation::topRight:
        xx = 0.25;
        yy = -0.60;
        break;
    case eOrientation::right:
        xx = 0.75;
        yy = -0.25;
        break;
    case eOrientation::bottomRight:
        xx = 0.85;
        yy = 0.15;
        break;
    case eOrientation::bottom:
        xx = 0.75;
        yy = 0.75;
        break;
    case eOrientation::bottomLeft:
        xx = 0.15;
        yy = 0.85;
        break;
    case eOrientation::left:
        xx = -0.25;
        yy = 0.75;
        break;
    case eOrientation::topLeft:
        xx = -0.75;
        yy = 0.1;
        break;
    case eOrientation::top:
        xx = -0.25;
        yy = -0.25;
        break;
    }

    std::shared_ptr<eTexture> tex;
    if(rCount <= 0) {
        tex = texs.fEmptyCart.getTexture(oi);
    } else {
        switch(rType) {
        case eResourceType::urchin:
            tex = texs.fUrchinCart[oi].getTexture(ci);
            break;
        case eResourceType::fish:
            tex = texs.fFishCart[oi].getTexture(ci);
            break;
        case eResourceType::meat:
            tex = texs.fMeatCart[oi].getTexture(ci);
            break;
        case eResourceType::cheese:
            tex = texs.fCheeseCart[oi].getTexture(ci);
            break;
        case eResourceType::carrots:
            tex = texs.fCarrotsCart[oi].getTexture(ci);
            break;
        case eResourceType::onions:
            tex = texs.fOnionsCart[oi].getTexture(ci);
            break;
        case eResourceType::wheat:
            tex = texs.fWheatCart[oi].getTexture(ci);
            break;
        case eResourceType::oranges:
            tex = texs.fOrangesCart[oi].getTexture(ci);
            break;
        case eResourceType::grapes:
            tex = texs.fGrapesCart[oi].getTexture(ci);
            break;
        case eResourceType::olives:
            tex = texs.fOlivesCart[oi].getTexture(ci);
            break;
        case eResourceType::wine:
            tex = texs.fWineCart[oi].getTexture(ci);
            break;
        case eResourceType::oliveOil:
            tex = texs.fOliveOilCart[oi].getTexture(ci);
            break;
        case eResourceType::fleece:
            tex = texs.fFleeceCart[oi].getTexture(ci);
            break;
        case eResourceType::bronze:
            tex = texs.fBronzeCart[oi].getTexture(ci);
            break;
        case eResourceType::orichalc:
            tex = texs.fOrichalcCart[oi].getTexture(ci);
            break;
        case eResourceType::armor:
            tex = texs.fArmorCart[oi].getTexture(ci);
            break;
        default:
            tex = texs.fEmptyCart.getTexture(oi);
            break;
        }
    }
    return {xx, yy, tex};
}

void eCartTransporter::setType(const eCartTransporterType t) {
    if(mType == t) return;
    mType = t;
    updateTextures();
    killAndClear(mOx);
    killAndClear(mTrailer);
    if(mType == eCartTransporterType::ox) {
        const auto t = tile();
        auto& board = getBoard();

        eCharacter* follow = this;

        if(!atlantean()) {
            const auto ox = e::make_shared<eOx>(board);
            ox->setAtlantean(atlantean());
            ox->setBothCityIds(cityId());
            mOx = ox;
            const auto aox = e::make_shared<eFollowAction>(follow, mOx.get());
            mOx->setAction(aox);
            mOx->changeTile(t);
            follow = mOx.get();
        }

        const auto trailer = e::make_shared<eTrailer>(board);
        trailer->setAtlantean(atlantean());
        trailer->setBothCityIds(cityId());
        mTrailer = trailer;
        mTrailer->setFollow(this);
        mTrailer->setBig(mBigTrailer);
        const auto atr = e::make_shared<eFollowAction>(follow, mTrailer.get());
        if(atlantean()) atr->setDistance(2);
        mTrailer->setAction(atr);
        mTrailer->changeTile(t);
    }
}

void eCartTransporter::setBigTrailer(const bool b) {
    mBigTrailer = b;
    if(mTrailer) mTrailer->setBig(b);
}

void eCartTransporter::setResourceValue(const eResourceType type,
                                        const int count) {
    auto& board = getBoard();
    const auto cid = cityId();
    const auto c = board.boardCityWithId(cid);

    if(mResourceType != eResourceType::none) {
        c->incWaitingCount(mResourceType, -mResourceCount);
    }

    mResourceType = type;
    mResourceCount = count;

    if(mResourceType != eResourceType::none) {
        c->incWaitingCount(mResourceType, mResourceCount);
    }
}

void eCartTransporter::setResource(const eResourceType type,
                                   const int count) {
    setResourceValue(type, count);

    if(mType == eCartTransporterType::ox ||
       mType == eCartTransporterType::basic) {
        switch(mResourceType) {
        case eResourceType::marble:
        case eResourceType::blackMarble:
        case eResourceType::wood:
        case eResourceType::sculpture:
            setType(eCartTransporterType::ox);
            break;
        default:
            setType(eCartTransporterType::basic);
        }
    }
    switch(mType) {
    case eCartTransporterType::food:
    case eCartTransporterType::fleece:
    case eCartTransporterType::oil:
    case eCartTransporterType::wine:
    case eCartTransporterType::arms:
    case eCartTransporterType::horse:
    case eCartTransporterType::chariot: {
        cleanupFollowers();
        const auto chr = eCartTransporterType::chariot;
        const int nFollPerRes = mType == chr ? 2 : 1;
        {
            const int iMax = mFollowers.size();
            for(int i = count*nFollPerRes; i < iMax; i++) {
                mFollowers.pop_back();
            }
        }
        const int iMax = count - mFollowers.size()/nFollPerRes;
    for(int i = 0; i < iMax; i++) {
        eCharacter* follow;
        if(mFollowers.empty()) {
            follow = this;
        } else {
            follow = this;
        }

            const auto t = tile();
            auto& board = getBoard();

            stdsptr<eCharacter> follower;
            if(mType == eCartTransporterType::horse) {
                follower = e::make_shared<eHorse>(board);
            } else if(mType == eCartTransporterType::chariot) {
                follower = e::make_shared<eHorse>(board);
                const auto aox = e::make_shared<eFollowAction>(
                                   follow, follower.get());
                follower->setAction(aox);
                follower->changeTile(t);
                mFollowers.push_back(follower);
                follow = follower.get();

                follower = e::make_shared<eChariot>(board);
            } else {
                follower = e::make_shared<ePorter>(board);
            }
            const auto aox = e::make_shared<eFollowAction>(
                               follow, follower.get());
            follower->setAction(aox);
            follower->changeTile(t);
            follower->setBothCityIds(cityId());
            follower->setAtlantean(atlantean());
            mFollowers.push_back(follower);
        }
    } break;
    default: break;
    }
}

int eCartTransporter::add(const eResourceType type, const int count) {
    if(count <= 0) return 0;
    const bool comp = mResourceType == type ||
                      mResourceCount <= 0;
    if(!comp) return 0;
    const int maxResource = eResourceTypeHelpers::transportSize(type, getBoard().doubleCartCapacity());
    const int r = std::clamp(mResourceCount + count, 0, maxResource);
    const int result = r - mResourceCount;
    setResource(type, r);
    return result;
}

int eCartTransporter::take(const eResourceType type, const int count) {
    if(count <= 0) return 0;
    if(type != mResourceType) return 0;

    const int result = std::clamp(count, 0, mResourceCount);
    setResource(type, mResourceCount - result);
    return result;
}

void eCartTransporter::setActionType(const eCharacterActionType t) {
    eCharacterBase::setActionType(t);
    if(mOx) mOx->setActionType(t);
}

void eCartTransporter::catchUp() {
    const auto cCatchUp = [](eCharacter* const c) {
        if(!c) return;
        const auto ca = c->action();
        if(const auto a = dynamic_cast<eFollowAction*>(ca)) {
            a->catchUp();
        }
    };
    if(mOx) cCatchUp(mOx);
    if(mTrailer) cCatchUp(mTrailer);
    for(const auto& f : mFollowers) {
        if(f) cCatchUp(f);
    }
}

void eCartTransporter::read(eReadStream& src) {
    eBasicPatroler::read(src);
    eSaveArchive ar(src);
    serialize(ar);
    updateTextures();
}

void eCartTransporter::write(eWriteStream& dst) const {
    eBasicPatroler::write(dst);
    eSaveArchive ar(dst);
    const_cast<eCartTransporter*>(this)->serialize(ar);
}

void eCartTransporter::serializeJson(eJsonArchive& ar) {
    eCharacter::serializeJson(ar);
    ar.field("count", mResourceCount);
    ar.field("mType", mType);
    ar.field("type", mResourceType);
    if(ar.reading()) {
        setResourceValue(mResourceType, mResourceCount);
    }
    ar.field("mSupports", mSupports);
    ar.field("mSupport", mSupport);
    ar.field("mWaiting", mWaiting);
    ar.field("mIsOx", mIsOx);
    ar.field("mBigTrailer", mBigTrailer);
    ar.field("mMaxDistance", mMaxDistance);
    if(ar.writing()) {
        eCharacter* rawOx = mOx.get();
        ar.characterRef("mOx", rawOx, getBoard());
        eCharacter* rawTrailer = mTrailer.get();
        ar.characterRef("mTrailer", rawTrailer, getBoard());
    } else {
        ar.characterRef("mOx", [this](eCharacter* c) {
            assignOx(mOx, c);
        }, getBoard());
        ar.characterRef("mTrailer", [this](eCharacter* c) {
            assignTrailer(mTrailer, c);
        }, getBoard());
    }
    int nf = ar.writing() ? static_cast<int>(mFollowers.size()) : 0;
    ar.field("nf", nf);
    if(ar.reading()) mFollowers.clear();
    for(int i = 0; i < nf; i++) {
        const auto key = "follower." + std::to_string(i);
        if(ar.writing()) {
            eCharacter* raw = mFollowers[i].get();
            ar.characterRef(key.c_str(), raw, getBoard());
        } else {
            ar.characterRef(key.c_str(), [this](eCharacter* c) {
                mFollowers.push_back(c);
            }, getBoard());
        }
    }
    if(ar.reading()) {
        ar.addCharPostFunc([this]() {
            cleanupFollowers();
        });
    }
    if(ar.reading()) updateTextures();
}

void eCartTransporter::serialize(eSaveArchive& ar) {
    int count = mResourceCount;
    ar.field("count", count);
    ar.field("mType", mType);
    eResourceType type = mResourceType;
    ar.field("type", type);
    if(ar.reading()) {
        setResourceValue(type, count);
    }
    ar.field("mSupports", mSupports);
    ar.field("mSupport", mSupport);
    ar.field("mWaiting", mWaiting);
    ar.field("mIsOx", mIsOx);
    ar.field("mBigTrailer", mBigTrailer);
    ar.field("mMaxDistance", mMaxDistance);
    if(ar.reading()) {
        ar.readStream().readCharacter(&getBoard(), [this](eCharacter* const c) {
            assignOx(mOx, c);
        });
        ar.readStream().readCharacter(&getBoard(), [this](eCharacter* const c) {
            assignTrailer(mTrailer, c);
        });
    } else {
        ar.writeStream().writeCharacter(mOx.get());
        ar.writeStream().writeCharacter(mTrailer.get());
    }
    int nf = static_cast<int>(mFollowers.size());
    ar.field("nf", nf);
    if(ar.reading()) mFollowers.clear();
    for(int i = 0; i < nf; i++) {
        if(ar.reading()) {
            ar.readStream().readCharacter(&getBoard(), [this](eCharacter* const c) {
                mFollowers.push_back(c);
            });
        } else {
            ar.writeStream().writeCharacter(mFollowers[i].get());
        }
    }
}

void eCartTransporter::updateTextures() {
    switch(mType) {
    case eCartTransporterType::basic: {
        eGameTextures::loadTransporter();
        eGameTextures::loadCart();
        setCharTextures(&eCharacterTextures::fTransporter);
    } break;
    case eCartTransporterType::ox: {
        if(atlantean()) {
            eGameTextures::loadElephant();
            setCharTextures(&eCharacterTextures::fElephant);
        } else {
            eGameTextures::loadOxHandler();
            setCharTextures(&eCharacterTextures::fOxHandler);
        }
    } break;
    case eCartTransporterType::food: {
        eGameTextures::loadFoodVendor();
        setCharTextures(&eCharacterTextures::fFoodVendor);
    } break;
    case eCartTransporterType::fleece: {
        eGameTextures::loadFleeceVendor();
        setCharTextures(&eCharacterTextures::fFleeceVendor);
    } break;
    case eCartTransporterType::oil: {
        eGameTextures::loadOilVendor();
        setCharTextures(&eCharacterTextures::fOilVendor);
    } break;
    case eCartTransporterType::wine: {
        eGameTextures::loadWineVendor();
        setCharTextures(&eCharacterTextures::fWineVendor);
    } break;
    case eCartTransporterType::arms: {
        eGameTextures::loadArmsVendor();
        setCharTextures(&eCharacterTextures::fArmsVendor);
    } break;
    case eCartTransporterType::horse: {
        eGameTextures::loadHorseVendor();
        setCharTextures(&eCharacterTextures::fHorseVendor);
    } break;
    case eCartTransporterType::chariot: {
        eGameTextures::loadChariotVendorCharacter();
        setCharTextures(&eCharacterTextures::fChariotVendor);
    } break;
    }
}

void eCartTransporter::cleanupFollowers() {
    for(int i = 0; i < (int)mFollowers.size(); i++) {
        const auto f = mFollowers[i];
        if(f) continue;
        for(int j = i; j < (int)mFollowers.size(); j++) {
            const auto ff = mFollowers[j];
            if(ff) ff->kill();
            mFollowers.erase(mFollowers.begin() + j);
            j--;
        }
        break;
    }
}

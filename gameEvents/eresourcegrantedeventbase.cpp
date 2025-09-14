#include "eresourcegrantedeventbase.h"

#include "engine/egameboard.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"

eResourceGrantedEventBase::eResourceGrantedEventBase(
        const eCityId cid,
        const eEvent giftCashAccepted,
        const eEvent giftAccepted,
        const eEvent giftPostponed,
        const eEvent giftRefused,
        const eEvent giftForfeited,
        const eEvent giftGranted,
        const eEvent giftLastChance,
        const eEvent giftInsufficientSpace,
        const eEvent giftPartialSpace,
        const eGameEventType type,
        const eGameEventBranch branch,
        eGameBoard& board) :
    eGameEvent(cid, type, branch, board),
    eCityEventValue(board),
    mGiftCashAccepted(giftCashAccepted),
    mGiftAccepted(giftAccepted),
    mGiftPostponed(giftPostponed),
    mGiftRefused(giftRefused),
    mGiftForfeited(giftForfeited),
    mGiftGranted(giftGranted),
    mGiftLastChance(giftLastChance),
    mGiftInsufficientSpace(giftInsufficientSpace),
    mGiftPartialSpace(giftPartialSpace) {}

void eResourceGrantedEventBase::initialize(
        const bool postpone,
        const eResourceType res,
        const int count,
        const stdsptr<eWorldCity> &c) {
    mPostpone = postpone;
    mResource = res;
    mCount = count;
    setSingleCity(c);
}

void eResourceGrantedEventBase::trigger() {
    chooseCity();
    if(!mCity) return;
    const auto board = gameBoard();
    if(!board) return;

    if(isMainEvent() && mPostpone) { // initial
        chooseType();
        chooseCount();
    }

    const auto pid = playerId();
    eEventData ed(pid);
    ed.fCity = mCity;
    int maxSpace = 0;
    const auto cids = board->playerCitiesOnBoard(pid);
    for(const auto cid : cids) {
        const int space = board->spaceForResource(cid, mResource);
        if(space > maxSpace) maxSpace = space;
        ed.fCityNames[cid] = board->cityName(cid);
        ed.fCSpaceCount[cid] = space;
    }
    ed.fResourceType = mResource;
    ed.fResourceCount = mCount;

    const auto postpone = [this, board]() {
        const auto branch = eGameEventBranch::child;
        const auto e = eGameEvent::sCreate(cityId(), type(), branch, *board);
        using eRGEB = eResourceGrantedEventBase;
        const auto ee = static_cast<eRGEB*>(e.get());
        ee->initialize(false, mResource, mCount, mCity);
        const auto date = board->date() + 31;
        e->initializeDate(date);
        addConsequence(e);
    };

    const auto acceptDrachmas = [this, board, pid]() { // accept
        const auto p = board->boardPlayerWithId(pid);
        if(p) p->incDrachmas(mCount, eFinanceTarget::giftsReceived);
        return mCount;
    };

    const auto acceptResource = [this, board](const eCityId cid) {
        const int a = board->addResource(cid, mResource, mCount);
        eEventData ed(cid);
        ed.fType = eMessageEventType::resourceGranted;
        ed.fCity = mCity;
        ed.fResourceType = mResource;
        ed.fResourceCount = a;
        if(mResource == eResourceType::drachmas) {
            board->event(mGiftCashAccepted, ed);
        } else {
            if(a == mCount) return;
            board->event(mGiftAccepted, ed);
        }
    };

    if(!isPersonPlayer()) {
        if(mResource == eResourceType::drachmas) {
            acceptDrachmas();
        } else if(maxSpace == 0) {
            if(mPostpone) {
                postpone();
            } else {
                board->event(mGiftForfeited, ed);
            }
        } else {
            for(const auto& c : ed.fCSpaceCount) {
                const int count = c.second;
                if(count == maxSpace) {
                    acceptResource(c.first);
                    break;
                }
            }
        }
    } else {
        if(maxSpace == 0) {
            ed.fType = eMessageEventType::resourceGranted;
            if(mPostpone) postpone();
        } else {
            ed.fType = eMessageEventType::requestTributeGranted;
            if(maxSpace != 0) {
                if(mResource == eResourceType::drachmas) {
                    ed.fA0 = acceptDrachmas;
                } else {
                    for(const auto cid : cids) {
                        ed.fCCA0[cid] = [acceptResource, cid]() { // accept
                            acceptResource(cid);
                        };
                    }
                }
            }

            if(mPostpone) {
                ed.fA1 = [this, board, postpone]() { // postpone
                    eEventData ed(playerId());
                    ed.fType = eMessageEventType::resourceGranted;
                    ed.fCity = mCity;
                    ed.fResourceType = mResource;
                    ed.fResourceCount = mCount;
                    board->event(mGiftPostponed, ed);

                    postpone();
                };
            }

            ed.fA2 = [this, board]() { // decline
                eEventData ed(playerId());
                ed.fType = eMessageEventType::resourceGranted;
                ed.fCity = mCity;
                ed.fResourceType = mResource;
                ed.fResourceCount = mCount;
                board->event(mGiftRefused, ed);
            };
        }
        if(!mPostpone) {
            if(maxSpace == 0) {
                board->event(mGiftForfeited, ed);
            } else if(maxSpace >= mCount) {
                board->event(mGiftGranted, ed);
            } else {
                board->event(mGiftLastChance, ed);
            }
        } else if(maxSpace == 0) {
            board->event(mGiftInsufficientSpace, ed);
        } else if(maxSpace >= mCount) {
            board->event(mGiftGranted, ed);
        } else {
            board->event(mGiftPartialSpace, ed);
        }
    }
}

void eResourceGrantedEventBase::write(eWriteStream& dst) const {
    eGameEvent::write(dst);
    eCityEventValue::write(dst);
    eResourceEventValue::write(dst);
    eCountEventValue::write(dst);
    dst << mPostpone;
}

void eResourceGrantedEventBase::read(eReadStream& src) {
    eGameEvent::read(src);
    eCityEventValue::read(src, *gameBoard());
    eResourceEventValue::read(src);
    eCountEventValue::read(src);
    src >> mPostpone;
}

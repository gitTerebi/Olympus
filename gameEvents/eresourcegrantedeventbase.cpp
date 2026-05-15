#include "eresourcegrantedeventbase.h"

#include "engine/e-game-board.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"
#include "fileIO/esavearchive.h"

class eResourceGrantedEventValues {
public:
    explicit eResourceGrantedEventValues(eResourceGrantedEventBase& event) :
        mEvent(event) {}

    void read(eReadStream& src) {
        mEvent.readEventValues(src);
    }

    void write(eWriteStream& dst) const {
        mEvent.writeEventValues(dst);
    }
private:
    eResourceGrantedEventBase& mEvent;
};

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
    ed.fEventRuntimeId = runtimeId();

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
        if(a == mCount) return;
        board->event(mGiftAccepted, ed);
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
                    ed.fPrimaryResponse = static_cast<int>(eResponse::accept);
                } else {
                    for(const auto cid : cids) {
                        ed.fCityConditionalResponses[cid] = static_cast<int>(eResponse::accept);
                    }
                }
            }

            if(mPostpone) {
                ed.fSecondaryResponse = static_cast<int>(eResponse::postpone);
            }

            ed.fTertiaryResponse = static_cast<int>(eResponse::decline);
            mAwaitingResponse = ed.fPrimaryResponse >= 0 ||
                                !ed.fCityConditionalResponses.empty() ||
                                ed.fSecondaryResponse >= 0 ||
                                ed.fTertiaryResponse >= 0;
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

bool eResourceGrantedEventBase::finished() const
{
    return eGameEvent::finished() && !mAwaitingResponse;
}

void eResourceGrantedEventBase::respond(const int response, const eCityId city)
{
    switch(static_cast<eResponse>(response)) {
    case eResponse::accept:
        accept(city);
        break;
    case eResponse::postpone:
        postpone();
        break;
    case eResponse::decline:
        decline();
        break;
    }
}

void eResourceGrantedEventBase::accept(const eCityId city)
{
    const auto board = gameBoard();
    if(!board) return;
    mAwaitingResponse = false;
    if(mResource == eResourceType::drachmas) {
        const auto p = board->boardPlayerWithId(playerId());
        if(p) p->incDrachmas(mCount, eFinanceTarget::giftsReceived);
        eEventData ed(playerId());
        ed.fType = eMessageEventType::resourceGranted;
        ed.fCity = mCity;
        ed.fResourceType = mResource;
        ed.fResourceCount = mCount;
        board->event(mGiftCashAccepted, ed);
        return;
    }
    const int a = board->addResource(city, mResource, mCount);
    eEventData ed(city);
    ed.fType = eMessageEventType::resourceGranted;
    ed.fCity = mCity;
    ed.fResourceType = mResource;
    ed.fResourceCount = a;
    if(a != mCount) {
        board->event(mGiftAccepted, ed);
    }
}

void eResourceGrantedEventBase::postpone()
{
    const auto board = gameBoard();
    if(!board) return;
    mAwaitingResponse = false;
    eEventData ed(playerId());
    ed.fType = eMessageEventType::resourceGranted;
    ed.fCity = mCity;
    ed.fResourceType = mResource;
    ed.fResourceCount = mCount;
    board->event(mGiftPostponed, ed);
    const auto branch = eGameEventBranch::child;
    const auto e = eGameEvent::sCreate(cityId(), type(), branch, *board);
    const auto ee = static_cast<eResourceGrantedEventBase*>(e.get());
    ee->initialize(false, mResource, mCount, mCity);
    const auto date = board->date() + 31;
    e->initializeDate(date);
    addConsequence(e);
}

void eResourceGrantedEventBase::decline()
{
    const auto board = gameBoard();
    if(!board) return;
    mAwaitingResponse = false;
    eEventData ed(playerId());
    ed.fType = eMessageEventType::resourceGranted;
    ed.fCity = mCity;
    ed.fResourceType = mResource;
    ed.fResourceCount = mCount;
    board->event(mGiftRefused, ed);
}

void eResourceGrantedEventBase::write(eWriteStream& dst) const {
    eGameEvent::write(dst);
    eSaveArchive ar(dst);
    const_cast<eResourceGrantedEventBase*>(this)->serialize(ar);
}

void eResourceGrantedEventBase::read(eReadStream& src) {
    eGameEvent::read(src);
    eSaveArchive ar(src);
    serialize(ar);
}

void eResourceGrantedEventBase::readEventValues(eReadStream& src) {
    eSaveArchive childAr(src);
    eCityEventValue::serialize(childAr, *gameBoard());
    eResourceEventValue::serialize(childAr);
    eCountEventValue::serialize(childAr);
}

void eResourceGrantedEventBase::writeEventValues(eWriteStream& dst) const {
    eSaveArchive childAr(dst);
    auto& self = const_cast<eResourceGrantedEventBase&>(*this);
    self.eCityEventValue::serialize(childAr, *gameBoard());
    self.eResourceEventValue::serialize(childAr);
    self.eCountEventValue::serialize(childAr);
}

void eResourceGrantedEventBase::serialize(eSaveArchive& ar) {
    eResourceGrantedEventValues values(*this);
    if(ar.writing()) {
        ar.objectField("eventValues", values);
    } else if(!ar.objectField("eventValues", values)) {
        // SAVE_COMPAT_LEGACY_FALLBACK: old saves stored these values inline.
        eCityEventValue::read(ar.readStream(), *gameBoard());
        eResourceEventValue::read(ar.readStream());
        eCountEventValue::read(ar.readStream());
    }
    ar.field("mPostpone", mPostpone);
    ar.field("mAwaitingResponse", mAwaitingResponse, false); // SAVE_COMPAT_OPTIONAL_FIELD
}

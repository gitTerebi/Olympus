#include "ask-for-aid-event.h"

#include "engine/emilitaryaid.h"
#include "engine/game-board.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"
#include "fileIO/save-archive.h"
#include "language.h"
#include "numbers.h"
#include "characters/soldier-banner.h"

AskForAidEvent::AskForAidEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        GameBoard& board) :
    eGameEvent(cid, eGameEventType::askForAid, branch, board) {}

void AskForAidEvent::trigger() {
    if(!mCity) return;
    const auto board = gameBoard();
    if(!board) return;
    const auto onCid = cityId();
    const auto fromCid = mCity->cityId();
    eEventData ed(onCid);
    ed.fCity = mCity;

    const auto date = board->date();
    const auto entryPoint = board->entryPoint(onCid);
    if(!entryPoint) return;

    if(mEnd) {
        const auto ma = board->militaryAid(onCid, mCity);
        if(ma->allDead()) {
            board->event(eEvent::aidDefeated, ed);
        } else {
            const auto& ivhs = board->invasionHandlers(onCid);
            const auto& ivs = board->invasions();
            const bool needed = !ivhs.empty() || !ivs.empty();
            const int days = date - mArrivalDate;
            if(!needed || (ivhs.empty() && days > 600)) {
                if(ma) {
                    ma->goBack();
                    const int t = mCity->troops();
                    mCity->setTroops(t + ma->count());
                    board->removeMilitaryAid(onCid, mCity);
                }
                board->event(eEvent::aidDeparts, ed);
            }
        }
        return;
    } else {
        const int t = mCity->troops();
        const int arrived = t/3;
        mCity->setTroops(t - arrived);

        mArrivalDate = date;

        const auto ma = std::make_shared<eMilitaryAid>();
        ma->fCity = mCity;
        ed.fTile = entryPoint;

        int rem = arrived;
        int nHorsemen = 0;
        if(arrived >= 28) {
            nHorsemen = (arrived - 12)/4;
        }
        rem -= nHorsemen;
        const int nRabble = rem/2;
        rem -= nRabble;
        const int nHoplites = rem;

        int remRabble = nRabble;
        while(remRabble > 0) {
            const auto b = e::make_shared<SoldierBanner>(
                               eBannerType::rockThrower, *board);
            b->setOnCityId(onCid);
            b->setCityId(fromCid);
            b->setMilitaryAid(true);
            b->backFromHome();
            for(int i = 0; i < Numbers::sSoldiersPerBanner && i < remRabble; i++) {
                b->createSoldier(entryPoint);
                b->incCount();
            }
            ma->fSoldiers.push_back(b);
            remRabble -= b->count();
        }

        int remHoplites = nHoplites;
        while(remHoplites > 0) {
            const auto b = e::make_shared<SoldierBanner>(
                               eBannerType::hoplite, *board);
            b->setOnCityId(onCid);
            b->setCityId(fromCid);
            b->setMilitaryAid(true);
            b->backFromHome();
            for(int i = 0; i < Numbers::sSoldiersPerBanner && i < remHoplites; i++) {
                b->createSoldier(entryPoint);
                b->incCount();
            }
            ma->fSoldiers.push_back(b);
            remHoplites -= b->count();
        }

        int remHorsemen = nHorsemen;
        while(remHorsemen > 0) {
            const auto b = e::make_shared<SoldierBanner>(
                               eBannerType::horseman, *board);
            b->setOnCityId(onCid);
            b->setCityId(fromCid);
            b->setMilitaryAid(true);
            b->backFromHome();
            for(int i = 0; i < Numbers::sSoldiersPerBanner && i < remHorsemen; i++) {
                b->createSoldier(entryPoint);
                b->incCount();
            }
            ma->fSoldiers.push_back(b);
            remHorsemen -= b->count();
        }

        std::vector<SoldierBanner*> banners;
        for(const auto& b : ma->fSoldiers) {
            banners.push_back(b.get());
        }

        const int ctx = entryPoint->x();
        const int cty = entryPoint->y();
        SoldierBanner::sPlace(banners, ctx, cty, *board, 3, 3);
        board->addMilitaryAid(onCid, ma);
    }

    const auto e = e::make_shared<AskForAidEvent>(
                       cityId(), eGameEventBranch::child, *board);
    e->setCity(mCity);
    e->setEnd(true);
    e->setArrivalDate(mArrivalDate);
    e->initializeDate(date + 150);
    addConsequence(e);
    board->event(eEvent::aidArrives, ed);
}

std::string AskForAidEvent::longName() const {
    return Language::text("request_aid_long_name");
}

void AskForAidEvent::serializeFields(SaveArchive& ar) {
    eGameEvent::serializeFields(ar);
    ar.worldCityField("city", worldBoard(), mCity);
    ar.dateField("arrivalDate", mArrivalDate);
    ar.field("end", mEnd, false);
}

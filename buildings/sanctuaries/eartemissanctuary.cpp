#include "eartemissanctuary.h"

#include <memory>

#include "engine/e-game-board.h"
#include "elanguage.h"
#include "enumbers.h"
#include "fileIO/esavearchive.h"

eArtemisSanctuary::eArtemisSanctuary(
        const int sw, const int sh, eGameBoard& board,
        const eCityId cid) :
    eSanctuaryWithWarriors(board, eBuildingType::templeArtemis,
                           sw, sh, 60, cid) {}

eAresSanctuary::eAresSanctuary(
        const int sw, const int sh, eGameBoard& board,
        const eCityId cid) :
    eSanctuaryWithWarriors(board, eBuildingType::templeAres,
                           sw, sh, 60, cid) {}

void eSanctuaryWithWarriors::timeChanged(const int by) {
    if(finished()) {
        int id = 0;
        while(mSoldierBanners.size() < 2) {
            auto& board = getBoard();
            const auto gt = godType();
            int string = -1;
            eBannerType bt;
            if(gt == eGodType::artemis) {
                bt = eBannerType::amazon;
                string = 30 + id;
            } else if(gt == eGodType::ares) {
                bt = eBannerType::aresWarrior;
                string = 32 + id;
            } else {
                return;
            }
            const auto name = eLanguage::zeusText(138, string);
            const auto b = e::make_shared<eSoldierBanner>(bt, board);
            for(int i = 0; i < eNumbers::sSoldiersPerAresArtemisBanner; i++) {
                b->incCount();
            }
            b->setName(name);
            mSoldierBanners.push_back(b);
            b->setBothCityIds(cityId());
            board.registerSoldierBanner(b);
            b->moveToPalace();
            id++;
        }
        mSoldierSpawn += by;
        const int ssr = 20000;
        if(mSoldierSpawn > ssr) {
            mSoldierSpawn -= ssr;
            for(const auto& b : mSoldierBanners) {
                const int count = b->count();
                if(count >= eNumbers::sSoldiersPerAresArtemisBanner) continue;
                b->incCount();
                break;
            }
        }
    }
    eSanctuary::timeChanged(by);
}

void eSanctuaryWithWarriors::serializeFields(eSaveArchive& ar) {
    eSanctuary::serializeFields(ar);
    auto& board = getBoard();
    if(ar.reading()) {
        const stdptr<eSanctuaryWithWarriors> tptr(this);
        auto banners = std::make_shared<std::vector<stdsptr<eSoldierBanner>>>();
        mSoldierBanners.clear();
        ar.countedArrayField("soldierBanners", 0,
            [&board, banners](eSaveArchive& itemAr, const int i) {
                if(i >= static_cast<int>(banners->size())) banners->resize(i + 1);
                itemAr.soldierBannerField("banner", &board, (*banners)[i]);
            });
        ar.addPostFunc([tptr, banners]() {
            if(!tptr) return;
            tptr->mSoldierBanners = *banners;
            for(int i = 0; i < static_cast<int>(tptr->mSoldierBanners.size()); i++) {
                const auto b = tptr->mSoldierBanners[i];
                if(!b) continue;
                const auto gt = tptr->godType();
                int string = -1;
                if(gt == eGodType::artemis) {
                    string = 30 + i;
                } else if(gt == eGodType::ares) {
                    string = 32 + i;
                } else {
                    continue;
                }
                const auto name = eLanguage::zeusText(138, string);
                b->setName(name);
            }
        }, "eSanctuaryWithWarriors::banners");
    } else {
        const int nb = static_cast<int>(mSoldierBanners.size());
        ar.countedArrayField("soldierBanners", nb,
            [&board, this](eSaveArchive& itemAr, const int i) {
                itemAr.soldierBannerField("banner", &board, mSoldierBanners[i]);
            });
    }
}

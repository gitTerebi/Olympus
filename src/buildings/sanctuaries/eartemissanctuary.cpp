#include "eartemissanctuary.h"

#include <memory>

#include "engine/game-board.h"
#include "characters/soldier-banner.h"
#include "language.h"
#include "numbers.h"
#include "fileIO/save-archive.h"

eArtemisSanctuary::eArtemisSanctuary(
        const int sw, const int sh, GameBoard& board,
        const eCityId cid) :
    eSanctuaryWithWarriors(board, eBuildingType::templeArtemis,
                           sw, sh, 60, cid) {}

eAresSanctuary::eAresSanctuary(
        const int sw, const int sh, GameBoard& board,
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
            if(gt == GodType::artemis) {
                bt = eBannerType::amazon;
                string = 30 + id;
            } else if(gt == GodType::ares) {
                bt = eBannerType::aresWarrior;
                string = 32 + id;
            } else {
                return;
            }
            const auto name = Language::zeusText(138, string);
            const auto b = e::make_shared<SoldierBanner>(bt, board);
            for(int i = 0; i < Numbers::sSoldiersPerAresArtemisBanner; i++) {
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
        const int ssr = 7 * Numbers::sDayLength;
        if(mSoldierSpawn > ssr) {
            mSoldierSpawn -= ssr;
            for(const auto& b : mSoldierBanners) {
                const int count = b->count();
                if(count >= Numbers::sSoldiersPerAresArtemisBanner) continue;
                b->incCount();
                break;
            }
        }
    }
    eSanctuary::timeChanged(by);
}

void eSanctuaryWithWarriors::serializeFields(SaveArchive& ar) {
    eSanctuary::serializeFields(ar);
    auto& board = getBoard();
    if(ar.reading()) {
        const stdptr<eSanctuaryWithWarriors> tptr(this);
        auto banners = std::make_shared<std::vector<std::shared_ptr<stdsptr<SoldierBanner>>>>();
        mSoldierBanners.clear();
        ar.countedArrayField("soldierBanners", 0,
            [&board, banners](SaveArchive& itemAr, const int i) {
                if(i >= static_cast<int>(banners->size())) banners->resize(i + 1);
                if(!(*banners)[i]) (*banners)[i] = std::make_shared<stdsptr<SoldierBanner>>();
                itemAr.soldierBannerField("banner", &board, *(*banners)[i]);
            });
        ar.addPostFunc([tptr, banners]() {
            if(!tptr) return;
            tptr->mSoldierBanners.clear();
            tptr->mSoldierBanners.reserve(banners->size());
            for(const auto& b : *banners) {
                tptr->mSoldierBanners.push_back(b ? *b : nullptr);
            }
            for(int i = 0; i < static_cast<int>(tptr->mSoldierBanners.size()); i++) {
                const auto b = tptr->mSoldierBanners[i];
                if(!b) continue;
                const auto gt = tptr->godType();
                int string = -1;
                if(gt == GodType::artemis) {
                    string = 30 + i;
                } else if(gt == GodType::ares) {
                    string = 32 + i;
                } else {
                    continue;
                }
                const auto name = Language::zeusText(138, string);
                b->setName(name);
            }
        }, "eSanctuaryWithWarriors::banners");
    } else {
        const int nb = static_cast<int>(mSoldierBanners.size());
        ar.countedArrayField("soldierBanners", nb,
            [&board, this](SaveArchive& itemAr, const int i) {
                itemAr.soldierBannerField("banner", &board, mSoldierBanners[i]);
            });
    }
}

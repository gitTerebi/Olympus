#include "eartemissanctuary.h"

#include "engine/e-game-board.h"
#include "elanguage.h"
#include "enumbers.h"
#include "fileIO/esavearchive.h"
#include "fileIO/ejsonarchive.h"

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
            b->moveToDefault();
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

void eSanctuaryWithWarriors::read(eReadStream& src) {
    eSanctuary::read(src);
    eSaveArchive ar(src);
    serialize(ar);
}

void eSanctuaryWithWarriors::write(eWriteStream& dst) const {
    eSanctuary::write(dst);
    eSaveArchive ar(dst);
    const_cast<eSanctuaryWithWarriors*>(this)->serialize(ar);
}

void eSanctuaryWithWarriors::serialize(eSaveArchive& ar) {
    auto& board = getBoard();
    int nb;
    if(ar.writing()) nb = mSoldierBanners.size();
    ar.field("nb", nb);
    if(ar.reading()) mSoldierBanners.clear();
    for(int i = 0; i < nb; i++) {
        if(ar.reading()) {
            ar.readStream().readSoldierBanner(&board, [this, i](const stdsptr<eSoldierBanner>& b) {
                if(!b) return;
                const auto gt = godType();
                int string = -1;
                if(gt == eGodType::artemis) {
                    string = 30 + i;
                } else if(gt == eGodType::ares) {
                    string = 32 + i;
                } else {
                    return;
                }
                const auto name = eLanguage::zeusText(138, string);
                b->setName(name);
                mSoldierBanners.push_back(b);
            });
        } else {
            ar.writeStream().writeSoldierBanner(mSoldierBanners[i].get());
        }
    }
}

void eSanctuaryWithWarriors::serializeJson(eJsonArchive& ar) {
    eSanctuary::serializeJson(ar);
    ar.field("mSoldierSpawn", mSoldierSpawn);
    // mSoldierBanners not saved in JSON path
}


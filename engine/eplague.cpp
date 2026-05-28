#include "eplague.h"

#include "buildings/small-house.h"
#include "ecityid.h"
#include "e-game-board.h"
#include "evectorhelpers.h"
#include "fileIO/esavearchive.h"

ePlague::ePlague(const eCityId cid, GameBoard& board) :
    mBoard(board), mCityId(cid) {}

void ePlague::randomSpread() {
    if(mHouses.empty()) return;
    std::random_shuffle(mHouses.begin(), mHouses.end());
    spreadFrom(mHouses[0]);
}

void ePlague::spreadFrom(SmallHouse* const h) {
    const auto hRect = h->tileRect();
    const int range = 5;
    const SDL_Rect contRect{hRect.x - range, hRect.y - range,
                            hRect.w + 2*range, hRect.h + 2*range};
    for(int x = contRect.x; x < contRect.x + contRect.w; x++) {
        for(int y = contRect.y; y < contRect.y + contRect.h; y++) {
            const auto t = mBoard.tile(x, y);
            if(!t) continue;
            const auto b = t->underBuilding();
            if(!b) continue;
            if(b->centerTile() != t) continue;
            const auto bt = b->type();
            if(bt != eBuildingType::commonHouse) continue;
            const auto ch = static_cast<SmallHouse*>(b);
            if(ch->plague()) continue;
            if(ch != h && eRand::rand() % 2) continue;
            mHouses.push_back(ch);
            ch->setPlague(true);
        }
    }
}

void ePlague::healAll() {
    for(const auto& h : mHouses) {
        h->setPlague(false);
    }
    mHouses.clear();
}

void ePlague::healHouse(SmallHouse* const h) {
    if(!h) return;
    h->setPlague(false);
    eVectorHelpers::remove(mHouses, h);
}

bool ePlague::hasHouse(SmallHouse* const h) const {
    return eVectorHelpers::contains(mHouses, h);
}

void ePlague::removeHouse(SmallHouse* const h) {
    eVectorHelpers::remove(mHouses, h);
}

void ePlague::serialize(eSaveArchive& ar) {
    const auto defaultCityId = mCityId;
    ar.field("cityId", mCityId, defaultCityId);
    ar.arrayField("houses", mHouses, [this](eSaveArchive& itemAr, SmallHouse*& h) {
        itemAr.buildingAsField("house", &mBoard, h);
    });
}

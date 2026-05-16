#include "eplague.h"

#include "buildings/esmallhouse.h"
#include "e-game-board.h"
#include "evectorhelpers.h"
#include "fileIO/esavearchive.h"

ePlague::ePlague(const eCityId cid, eGameBoard& board) :
    mBoard(board), mCityId(cid) {}

void ePlague::randomSpread() {
    if(mHouses.empty()) return;
    std::random_shuffle(mHouses.begin(), mHouses.end());
    spreadFrom(mHouses[0]);
}

void ePlague::spreadFrom(eSmallHouse* const h) {
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
            const auto ch = static_cast<eSmallHouse*>(b);
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

void ePlague::healHouse(eSmallHouse* const h) {
    if(!h) return;
    h->setPlague(false);
    eVectorHelpers::remove(mHouses, h);
}

bool ePlague::hasHouse(eSmallHouse* const h) const {
    return eVectorHelpers::contains(mHouses, h);
}

void ePlague::removeHouse(eSmallHouse* const h) {
    eVectorHelpers::remove(mHouses, h);
}

void ePlague::read(eReadStream& src) {
    eSaveArchive ar(src);
    serialize(ar);
}

void ePlague::write(eWriteStream& dst) const {
    eSaveArchive ar(dst);
    const_cast<ePlague*>(this)->serialize(ar);
}

void ePlague::serialize(eSaveArchive& ar) {
    ar.field("cityId", mCityId);
    int houseCount = ar.writing() ? static_cast<int>(mHouses.size()) : 0;
    ar.field("houses.count", houseCount);
    if(ar.reading()) mHouses.clear();
    for(int i = 0; i < houseCount; i++) {
        ar.payloadField(("house." + std::to_string(i)).c_str(),
            [this, i](eWriteStream& dst) { dst.writeBuilding(mHouses[i]); },
            [this](eReadStream& src) {
                src.readBuilding(&mBoard, [this](eBuilding* const b) {
                    const auto ch = static_cast<eSmallHouse*>(b);
                    mHouses.push_back(ch);
                });
            });
    }
}

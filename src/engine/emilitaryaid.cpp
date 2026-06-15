#include "emilitaryaid.h"

#include <algorithm>
#include <memory>

#include "characters/soldier-banner.h"
#include "fileIO/save-archive.h"

bool eMilitaryAid::count() const {
    int c = 0;
    for(const auto& s : fSoldiers) {
        c += s->count();
    }
    return c;
}

void eMilitaryAid::goBack() {
    for(const auto& s : fSoldiers) {
        s->goAbroad();
    }
    fSoldiers.clear();
}

void eMilitaryAid::serialize(SaveArchive& ar, GameBoard* board) {
    ar.worldCityField("city", board, fCity);
    if(ar.reading()) {
        auto soldiers = std::make_shared<std::vector<stdsptr<SoldierBanner>>>();
        ar.arrayField("soldiers", *soldiers, [board](SaveArchive& itemAr, auto& soldier) {
            itemAr.soldierBanner(board, soldier);
        });
        ar.addPostFunc([this, soldiers]() {
            fSoldiers = *soldiers;
            fSoldiers.erase(std::remove_if(fSoldiers.begin(), fSoldiers.end(),
                                           [](const auto& s) { return !s; }),
                            fSoldiers.end());
        }, "eMilitaryAid::soldiers");
    } else {
        ar.arrayField("soldiers", fSoldiers, [board](SaveArchive& itemAr, auto& soldier) {
            itemAr.soldierBanner(board, soldier);
        });
    }
}

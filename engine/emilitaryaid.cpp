#include "emilitaryaid.h"

#include <algorithm>
#include <memory>

#include "characters/esoldierbanner.h"
#include "fileIO/esavearchive.h"

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

void eMilitaryAid::write(eWriteStream& dst) {
    eSaveArchive ar(dst);
    serialize(ar, nullptr);
}

void eMilitaryAid::read(eReadStream& src, eGameBoard* const board) {
    eSaveArchive ar(src);
    serialize(ar, board);
}

void eMilitaryAid::serialize(eSaveArchive& ar, eGameBoard* board) {
    ar.payloadField("city",
        [this](eWriteStream& dst) { dst.writeCity(fCity.get()); },
        [this, board](eReadStream& src) {
            src.readCity(board, [this](const stdsptr<eWorldCity>& c) {
                fCity = c;
            });
        });
    if(ar.reading()) {
        auto soldiers = std::make_shared<std::vector<stdsptr<eSoldierBanner>>>();
        ar.arrayField("soldiers", *soldiers, [board](eSaveArchive& itemAr, auto& soldier) {
            itemAr.soldierBanner(board, soldier);
        });
        ar.addPostFunc([this, soldiers]() {
            fSoldiers = *soldiers;
            fSoldiers.erase(std::remove_if(fSoldiers.begin(), fSoldiers.end(),
                                           [](const auto& s) { return !s; }),
                            fSoldiers.end());
        }, "eMilitaryAid::soldiers");
    } else {
        ar.arrayField("soldiers", fSoldiers, [board](eSaveArchive& itemAr, auto& soldier) {
            itemAr.soldierBanner(board, soldier);
        });
    }
}

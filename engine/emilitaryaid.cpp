#include "emilitaryaid.h"

#include <algorithm>

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
    if(ar.reading()) {
        ar.readStream().readCity(board, [this](const stdsptr<eWorldCity>& c) {
            fCity = c;
        });
    } else {
        ar.writeStream().writeCity(fCity.get());
    }
    ar.arrayField("soldiers", fSoldiers, [board](eSaveArchive& ar, auto& soldier) {
        ar.soldierBanner(board, soldier);
    });
    if(ar.reading()) {
        fSoldiers.erase(std::remove_if(fSoldiers.begin(), fSoldiers.end(),
                                       [](const auto& s) { return !s; }),
                        fSoldiers.end());
    }
}

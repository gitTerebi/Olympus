#include "emilitaryaid.h"

#include "characters/esoldierbanner.h"

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
    dst.writeCity(fCity.get());
    dst << fSoldiers.size();
    for(const auto& s : fSoldiers) {
        dst.writeSoldierBanner(s.get());
    }
}

void eMilitaryAid::read(eReadStream& src, eGameBoard* const board) {
    src.readCity(board, [this](const stdsptr<eWorldCity>& c) {
        fCity = c;
    });
    int ns;
    src >> ns;
    for(int i = 0; i < ns; i++) {
        src.readSoldierBanner(board, [this](const stdsptr<eSoldierBanner>& b) {
            fSoldiers.push_back(b);
        });
    }
}

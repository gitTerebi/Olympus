#include "emilitaryaid.h"

#include "characters/esoldierbanner.h"
#include "fileIO/esavearchive.h"
#include "fileIO/ejsonarchive.h"

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

void eMilitaryAid::serializeJson(eJsonArchive& ar, eGameBoard& board) {
    ar.cityRef("fCity", fCity, board);
    int ns = static_cast<int>(fSoldiers.size());
    ar.field("ns", ns);
    if(ar.reading()) fSoldiers.clear();
    for(int i = 0; i < ns; i++) {
        const auto key = "s" + std::to_string(i);
        if(ar.reading()) {
            ar.soldierBannerRef(key.c_str(), [this](const stdsptr<eSoldierBanner>& b) {
                if(b) fSoldiers.push_back(b);
            }, board);
        } else {
            int ioid = fSoldiers[i] ? fSoldiers[i]->ioID() : -1;
            ar.field(key.c_str(), ioid, -1);
        }
    }
}

void eMilitaryAid::serialize(eSaveArchive& ar, eGameBoard* board) {
    if(ar.reading()) {
        ar.readStream().readCity(board, [this](const stdsptr<eWorldCity>& c) {
            fCity = c;
        });
    } else {
        ar.writeStream().writeCity(fCity.get());
    }
    int ns = fSoldiers.size();
    ar.field("ns", ns);
    if(ar.reading()) fSoldiers.clear();
    for(int i = 0; i < ns; i++) {
        if(ar.reading()) {
            ar.readStream().readSoldierBanner(board, [this](const stdsptr<eSoldierBanner>& b) {
                if(b) fSoldiers.push_back(b);
            });
        } else {
            ar.writeStream().writeSoldierBanner(fSoldiers[i].get());
        }
    }
}

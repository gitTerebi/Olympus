#ifndef EGODQUEST_H
#define EGODQUEST_H

#include "characters/gods/egod.h"
#include "characters/heroes/ehero.h"
#include "fileIO/esavearchive.h"

struct eGodQuest {
    static eHeroType sDefaultHero(
            const eGodType gt, const eGodQuestId gqi);

    void serialize(eSaveArchive& ar) {
        ar.field("god", fGod);
        ar.field("questId", fId);
        ar.field("hero", fHero);
    }

    void write(eWriteStream& dst) const {
        eSaveArchive ar(dst);
        const_cast<eGodQuest*>(this)->serialize(ar);
    }

    void read(eReadStream& src) {
        eSaveArchive ar(src);
        serialize(ar);
    }

    bool operator==(const eGodQuest& o) const {
        return fGod == o.fGod &&
               fId == o.fId &&
               fHero == o.fHero;
    }

    std::string name() const;

    eGodType fGod = eGodType::zeus;
    eGodQuestId fId = eGodQuestId::godQuest1;
    eHeroType fHero = eGodQuest::sDefaultHero(fGod, fId);
};

#endif // EGODQUEST_H

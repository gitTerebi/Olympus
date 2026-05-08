#ifndef EGODQUEST_H
#define EGODQUEST_H

#include "characters/gods/egod.h"
#include "characters/heroes/ehero.h"
#include "fileIO/esavearchive.h"

struct eGodQuest {
    static eHeroType sDefaultHero(
            const eGodType gt, const eGodQuestId gqi);

    void write(eWriteStream& dst) const {
        eSaveArchive ar(dst);
        ar.field("god", const_cast<eGodType&>(fGod));
        ar.field("id", const_cast<eGodQuestId&>(fId));
        ar.field("hero", const_cast<eHeroType&>(fHero));
    }

    void read(eReadStream& src) {
        eSaveArchive ar(src);
        ar.field("god", fGod);
        ar.field("id", fId);
        ar.field("hero", fHero);
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

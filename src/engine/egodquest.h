#ifndef EGODQUEST_H
#define EGODQUEST_H

#include "characters/gods/god.h"
#include "characters/heroes/ehero.h"
#include "fileIO/esavearchive.h"

struct eGodQuest {
    static eHeroType sDefaultHero(
            const GodType gt, const GodQuestId gqi);

    void serialize(eSaveArchive& ar) {
        ar.field("god", fGod, GodType::zeus);
        ar.field("questId", fId, GodQuestId::godQuest1);
        ar.field("hero", fHero, eGodQuest::sDefaultHero(fGod, fId));
    }

    bool operator==(const eGodQuest& o) const {
        return fGod == o.fGod &&
               fId == o.fId &&
               fHero == o.fHero;
    }

    std::string name() const;

    GodType fGod = GodType::zeus;
    GodQuestId fId = GodQuestId::godQuest1;
    eHeroType fHero = eGodQuest::sDefaultHero(fGod, fId);
};

#endif // EGODQUEST_H

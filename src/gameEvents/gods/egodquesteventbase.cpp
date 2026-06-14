#include "egodquesteventbase.h"

#include "fileIO/esavearchive.h"

void eGodQuestEventBase::setGod(const GodType g) {
    mQuest.fGod = g;
}

void eGodQuestEventBase::setId(const GodQuestId id) {
    mQuest.fId = id;
}

void eGodQuestEventBase::setHero(const eHeroType h) {
    mQuest.fHero = h;
}

void eGodQuestEventBase::serializeFields(eSaveArchive& ar) {
    eGameEvent::serializeFields(ar);
    ar.archiveField("quest", [this](eSaveArchive& childAr) {
        mQuest.serialize(childAr);
    });
}

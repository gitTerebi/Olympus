#include "egodquesteventbase.h"

#include "fileIO/esavearchive.h"
#include "fileIO/ejsonarchive.h"

void eGodQuestEventBase::setGod(const eGodType g) {
    mQuest.fGod = g;
}

void eGodQuestEventBase::setId(const eGodQuestId id) {
    mQuest.fId = id;
}

void eGodQuestEventBase::setHero(const eHeroType h) {
    mQuest.fHero = h;
}

void eGodQuestEventBase::write(eWriteStream& dst) const {
    eGameEvent::write(dst);
    eSaveArchive ar(dst);
    const_cast<eGodQuestEventBase*>(this)->serialize(ar);
}

void eGodQuestEventBase::read(eReadStream& src) {
    eGameEvent::read(src);
    eSaveArchive ar(src);
    serialize(ar);
}

void eGodQuestEventBase::serialize(eSaveArchive& ar) {
    if(ar.reading()) {
        mQuest.read(ar.readStream());
    } else {
        mQuest.write(ar.writeStream());
    }
}

void eGodQuestEventBase::serializeJson(eJsonArchive& ar) {
    eGameEvent::serializeJson(ar);
    ar.field("fGod", mQuest.fGod);
    ar.field("fId", mQuest.fId);
    ar.field("fHero", mQuest.fHero);
}

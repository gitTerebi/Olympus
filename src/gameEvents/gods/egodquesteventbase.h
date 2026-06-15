#ifndef EGODQUESTEVENTBASE_H
#define EGODQUESTEVENTBASE_H

#include "../egameevent.h"

#include "engine/egodquest.h"

class SaveArchive;

class eGodQuestEventBase : public eGameEvent {
public:
    using eGameEvent::eGameEvent;

    const eGodQuest& godQuest() const { return mQuest; }

    GodType god() const { return mQuest.fGod; }
    void setGod(const GodType g);

    GodQuestId id() const { return mQuest.fId; }
    void setId(const GodQuestId id);

    eHeroType hero() const { return mQuest.fHero; }
    void setHero(const eHeroType h);
protected:
    void serializeFields(SaveArchive& ar) override;
private:
    eGodQuest mQuest;
};

#endif // EGODQUESTEVENTBASE_H

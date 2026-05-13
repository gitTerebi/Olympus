#ifndef EMONSTEREVENTVALUE_H
#define EMONSTEREVENTVALUE_H

#include "characters/monsters/emonstertype.h"
#include "fileIO/ejsonarchive.h"

class eWriteStream;
class eReadStream;

class eSaveArchive;
class eMonsterEventValue {
public:
    void write(eWriteStream& dst) const;
    void serialize(eSaveArchive& ar);
    void read(eReadStream& src);

    void setMonster(const eMonsterType m) { mMonster = m; }
    eMonsterType monster() const { return mMonster; }
protected:
    eMonsterType mMonster = eMonsterType::calydonianBoar;
    virtual void serializeJson(eJsonArchive& ar) {
        ar.field("mMonster", mMonster);
}

};

#endif // EMONSTEREVENTVALUE_H

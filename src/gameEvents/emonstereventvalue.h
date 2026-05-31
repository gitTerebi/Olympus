#ifndef EMONSTEREVENTVALUE_H
#define EMONSTEREVENTVALUE_H

#include "characters/monsters/emonstertype.h"

class eWriteStream;
class eReadStream;

class eSaveArchive;
class eMonsterEventValue {
public:
    void serialize(eSaveArchive& ar);

    void setMonster(const eMonsterType m) { mMonster = m; }
    eMonsterType monster() const { return mMonster; }
protected:
    eMonsterType mMonster = eMonsterType::calydonianBoar;
};

#endif // EMONSTEREVENTVALUE_H

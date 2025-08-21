#ifndef EMONSTERSEVENTVALUE_H
#define EMONSTERSEVENTVALUE_H

#include <vector>

#include "characters/monsters/emonstertype.h"

class eWriteStream;
class eReadStream;

struct eMonsterTypeValid {
    eMonsterType fType = eMonsterType::calydonianBoar;
    bool fValid = false;
};

class eMonstersEventValue {
public:
    eMonstersEventValue();

    const std::vector<eMonsterTypeValid>& monsterTypes() const
    { return mMonsters; }
    eMonsterType monsterType() const { return mMonster; }
    eMonsterType monsterType(const int id, bool& valid) const;
    void setMonsterType(const int id, const eMonsterType type);
    void setMonsterTypes(const std::vector<eMonsterType>& types);

    void write(eWriteStream& dst) const;
    void read(eReadStream& src);
protected:
    void chooseMonster();

    std::vector<eMonsterTypeValid> mMonsters = {{}, {}, {}};

    eMonsterType mMonster;
};

#endif // EMONSTERSEVENTVALUE_H

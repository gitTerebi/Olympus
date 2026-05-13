#ifndef EMONSTERSEVENTVALUE_H
#define EMONSTERSEVENTVALUE_H

#include <vector>

#include "characters/monsters/emonstertype.h"
#include "fileIO/ejsonarchive.h"

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
    bool chooseMonster(const std::vector<eMonsterType>& skip);

    std::vector<eMonsterTypeValid> mMonsters = {{}, {}, {}};

    eMonsterType mMonster;
    virtual void serializeJson(eJsonArchive& ar) {
        ar.field("mMonster", mMonster);
        const char* const typeNames[] = { "mMonster0Type", "mMonster1Type", "mMonster2Type" };
        const char* const validNames[] = { "mMonster0Valid", "mMonster1Valid", "mMonster2Valid" };
        for(int i = 0; i < 3; i++) {
            auto& v = mMonsters[i];
            ar.field(typeNames[i], v.fType);
            ar.field(validNames[i], v.fValid);
        }
    }

};

#endif // EMONSTERSEVENTVALUE_H

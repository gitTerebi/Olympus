#include "emonsterseventvalue.h"

#include "erand.h"

#include "fileIO/ewritestream.h"
#include "fileIO/ereadstream.h"
#include "fileIO/esavearchive.h"

#include "evectorhelpers.h"

eMonstersEventValue::eMonstersEventValue() {}

eMonsterType eMonstersEventValue::monsterType(const int id, bool &valid) const {
    auto& v = mMonsters[id];
    valid = v.fValid;
    return v.fType;
}

void eMonstersEventValue::setMonsterType(const int id, const eMonsterType type) {
    auto& v = mMonsters[id];
    v.fType = type;
    v.fValid = true;
}

void eMonstersEventValue::setMonsterTypes(const std::vector<eMonsterType> &types) {
    const int iMax = types.size();
    for(int i = 0; i < iMax && i < 3; i++) {
        setMonsterType(i, types[i]);
    }
}

void eMonstersEventValue::serialize(eSaveArchive& ar) {
    ar.field("monster", mMonster, eMonsterType::calydonianBoar);
    const char* const typeNames[] = {
        "monster0Type", "monster1Type", "monster2Type"
    };
    const char* const validNames[] = {
        "monster0Valid", "monster1Valid", "monster2Valid"
    };
    for(int i = 0; i < 3; i++) {
        auto& v = mMonsters[i];
        ar.field(typeNames[i], v.fType, eMonsterType::calydonianBoar);
        ar.field(validNames[i], v.fValid, false);
    }
}

bool eMonstersEventValue::chooseMonster(const std::vector<eMonsterType> &skip) {
    std::vector<eMonsterType> types;
    for(const auto& v : mMonsters) {
        if(!v.fValid) continue;
        const bool c = eVectorHelpers::contains(skip, v.fType);
        if(c) continue;
        types.push_back(v.fType);
    }
    if(types.empty()) {
        printf("No monster types to choose from\n");
        return false;
    }
    const int typeId = eRand::rand() % types.size();
    mMonster = types[typeId];
    return true;
}

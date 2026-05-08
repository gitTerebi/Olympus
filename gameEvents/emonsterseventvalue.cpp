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

void eMonstersEventValue::write(eWriteStream &dst) const {
    eSaveArchive ar(dst);
    ar.field("mMonster", const_cast<eMonsterType&>(mMonster));

    const char* const typeNames[] = {
        "mMonster0Type", "mMonster1Type", "mMonster2Type"
    };
    const char* const validNames[] = {
        "mMonster0Valid", "mMonster1Valid", "mMonster2Valid"
    };
    for(int i = 0; i < 3; i++) {
        auto& v = const_cast<eMonsterTypeValid&>(mMonsters[i]);
        ar.field(typeNames[i], v.fType);
        ar.field(validNames[i], v.fValid);
    }
}

void eMonstersEventValue::read(eReadStream &src) {
    eSaveArchive ar(src);
    ar.field("mMonster", mMonster);

    const char* const typeNames[] = {
        "mMonster0Type", "mMonster1Type", "mMonster2Type"
    };
    const char* const validNames[] = {
        "mMonster0Valid", "mMonster1Valid", "mMonster2Valid"
    };
    for(int i = 0; i < 3; i++) {
        auto& v = mMonsters[i];
        ar.field(typeNames[i], v.fType);
        ar.field(validNames[i], v.fValid);
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

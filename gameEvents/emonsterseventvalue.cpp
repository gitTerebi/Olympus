#include "emonsterseventvalue.h"

#include "erand.h"

#include "fileIO/ewritestream.h"
#include "fileIO/ereadstream.h"

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
    dst << mMonster;

    for(const auto& v : mMonsters) {
        dst << v.fType;
        dst << v.fValid;
    }
}

void eMonstersEventValue::read(eReadStream &src) {
    src >> mMonster;

    for(int i = 0; i < 3; i++) {
        auto& v = mMonsters[i];
        src >> v.fType;
        src >> v.fValid;
    }
}

void eMonstersEventValue::chooseMonster() {
    std::vector<eMonsterType> types;
    for(const auto& v : mMonsters) {
        if(!v.fValid) continue;
        types.push_back(v.fType);
    }
    if(types.empty()) {
        printf("No monster types to choose from\n");
        return;
    }
    const int typeId = eRand::rand() % types.size();
    mMonster = types[typeId];
}

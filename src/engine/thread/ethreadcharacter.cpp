#include "ethreadcharacter.h"

eThreadCharacter::eThreadCharacter() :
    eCharacterBase(eCharacterType::none) {}

void eThreadCharacter::load(eCharacterBase* const character) {
    setBusy(character->busy());
    setType(character->type());
    setCityId(character->cityId());
    setOnCityId(character->onCityId());
    setHP(character->hp());
    setActionType(character->actionType());
}

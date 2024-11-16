#include "ethreadcharacter.h"

eThreadCharacter::eThreadCharacter() :
    eCharacterBase(eCharacterType::none) {}

void eThreadCharacter::load(eCharacterBase* const character) {
    setBusy(character->busy());
    setType(character->type());
    setCityId(character->cityId());
    setHP(character->hp());
    setActionType(character->actionType());
}

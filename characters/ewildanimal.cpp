#include "ewildanimal.h"

#include "spawners/espawner.h"
#include "fileIO/esavearchive.h"

eWildAnimal::eWildAnimal(GameBoard& board,
                         const eCharTexs charTexs,
                         const eCharacterType type) :
    Animal(board, charTexs, type) {}

eWildAnimal::~eWildAnimal() {
    if(mSpawner) mSpawner->decCount();
}

void eWildAnimal::serializeFields(eSaveArchive& ar) {
    Animal::serializeFields(ar);
    ar.bannerField("spawner", &getBoard(), mSpawner);
}

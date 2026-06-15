#include "ewildanimal.h"

#include "spawners/spawner.h"
#include "fileIO/save-archive.h"

eWildAnimal::eWildAnimal(GameBoard& board,
                         const eCharTexs charTexs,
                         const eCharacterType type) :
    Animal(board, charTexs, type) {}

eWildAnimal::~eWildAnimal() {
    if(mSpawner) mSpawner->decCount();
}

void eWildAnimal::serializeFields(SaveArchive& ar) {
    Animal::serializeFields(ar);
    ar.bannerField("spawner", &getBoard(), mSpawner);
}

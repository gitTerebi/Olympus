#ifndef EWILDANIMAL_H
#define EWILDANIMAL_H

#include "animal.h"

#include "textures/character-textures.h"

class Spawner;
class SaveArchive;

class eWildAnimal : public Animal {
public:
    eWildAnimal(GameBoard& board,
                const eCharTexs charTexs,
                const eCharacterType type);
    ~eWildAnimal();

    void setSpawner(Spawner* const s)
    { mSpawner = s; }

protected:
    void serializeFields(SaveArchive& ar) override;
private:
    Spawner* mSpawner = nullptr;
};

#endif // EWILDANIMAL_H

#ifndef EWILDANIMAL_H
#define EWILDANIMAL_H

#include "animal.h"

#include "textures/echaractertextures.h"

class eSpawner;
class eSaveArchive;

class eWildAnimal : public Animal {
public:
    eWildAnimal(GameBoard& board,
                const eCharTexs charTexs,
                const eCharacterType type);
    ~eWildAnimal();

    void setSpawner(eSpawner* const s)
    { mSpawner = s; }

protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    eSpawner* mSpawner = nullptr;
};

#endif // EWILDANIMAL_H

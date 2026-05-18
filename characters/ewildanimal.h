#ifndef EWILDANIMAL_H
#define EWILDANIMAL_H

#include "eanimal.h"

#include "textures/echaractertextures.h"

class eSpawner;
class eSaveArchive;

class eWildAnimal : public eAnimal {
public:
    eWildAnimal(eGameBoard& board,
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

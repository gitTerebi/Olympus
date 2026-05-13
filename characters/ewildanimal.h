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

    void read(eReadStream& src);
    void write(eWriteStream& dst) const;
    void serializeJson(eJsonArchive& ar) override;
private:
    void serialize(eSaveArchive& ar);

    eSpawner* mSpawner = nullptr;
};

#endif // EWILDANIMAL_H

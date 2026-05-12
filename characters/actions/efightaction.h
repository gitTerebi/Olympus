#ifndef EFIGHTACTION_H
#define EFIGHTACTION_H

#include "echaracteraction.h"
#include "characters/echaracter.h"

class eSaveArchive;
class eJsonArchive;

class eFightAction : public eCharacterAction {
public:
    eFightAction(eCharacter* const c,
                 eCharacter* const o = nullptr);

    void increment(const int by) override;

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;
    void serializeJson(eJsonArchive& ar) override;
private:
    void serialize(eSaveArchive& ar);

    stdptr<eCharacter> mOpponent;

    int mTime = 0;
};

#endif // EFIGHTACTION_H

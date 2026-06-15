#ifndef EFIGHTINGCHARACTER_H
#define EFIGHTINGCHARACTER_H

class ReadStream;
class WriteStream;
class SaveArchive;

class eCharacter;
class FightingAction;

class eFightingCharacter {
public:
    eFightingCharacter(eCharacter* const c);

    void serializeFields(SaveArchive& ar);

    FightingAction* fightingAction() const;
private:
    eCharacter* const mChar;
};

#endif // EFIGHTINGCHARACTER_H

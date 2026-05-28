#ifndef EFIGHTINGCHARACTER_H
#define EFIGHTINGCHARACTER_H

class eReadStream;
class eWriteStream;
class eSaveArchive;

class eCharacter;
class eFightingAction;

class eFightingCharacter {
public:
    eFightingCharacter(eCharacter* const c);

    void serializeFields(eSaveArchive& ar);

    eFightingAction* fightingAction() const;
private:
    eCharacter* const mChar;
};

#endif // EFIGHTINGCHARACTER_H

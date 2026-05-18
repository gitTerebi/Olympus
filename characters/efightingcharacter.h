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

    int range() const { return mRange; }
    void setRange(const int r) { mRange = r; }
private:
    eCharacter* const mChar;

    int mRange = 0;
};

#endif // EFIGHTINGCHARACTER_H

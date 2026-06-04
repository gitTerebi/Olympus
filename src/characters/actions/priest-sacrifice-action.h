#ifndef PRIEST_SACRIFICE_ACTION_H
#define PRIEST_SACRIFICE_ACTION_H

#include "eactionwithcomeback.h"
#include "characters/echaracterbase.h"

class eTempleAltarBuilding;
class eDomesticatedAnimal;
class eSaveArchive;

enum class ePriestSacrificeStage {
    idle,
    findingAnimal,
    movingToAnimal,
    atAnimal,
    returning
};

class PriestSacrificeAction : public eActionWithComeback {
    friend class ePrSA_atAnimalFinish;
public:
    PriestSacrificeAction(eCharacter* const c,
                          eTempleAltarBuilding* const altar);
    PriestSacrificeAction(eCharacter* const c);
    ~PriestSacrificeAction();

    bool decide() override;
protected:
    void serializeFields(eSaveArchive& ar) override;
    void resumeFromSavedState() override;
private:
    eDomesticatedAnimal* findAnimal() const;
    void toFindAnimal();
    void toMovingToAnimal(eDomesticatedAnimal* a);
    void toAtAnimal();
    void toReturning();
    void toIdle();

    stdptr<eTempleAltarBuilding> mAltar;
    stdptr<eDomesticatedAnimal> mTargetAnimal;
    stdptr<eCharacter> mGhost;

    ePriestSacrificeStage mStage = ePriestSacrificeStage::idle;
    int mSpawnTimer = 0;
};

class ePrSA_atAnimalFinish : public eCharActFunc {
public:
    ePrSA_atAnimalFinish(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::PrSA_atAnimalFinish) {}
    ePrSA_atAnimalFinish(GameBoard& board,
                         PriestSacrificeAction* const a) :
        eCharActFunc(board, eCharActFuncType::PrSA_atAnimalFinish),
        mAptr(a) {}

    void call() override {
        if(!mAptr) return;
        mAptr->toReturning();
    }
protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterActionAsField("action", &board(), mAptr);
    }
private:
    stdptr<PriestSacrificeAction> mAptr;
};

#endif // PRIEST_SACRIFICE_ACTION_H

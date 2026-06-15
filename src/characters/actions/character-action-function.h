#ifndef CHARACTER_ACTION_FUNCTION_H
#define CHARACTER_ACTION_FUNCTION_H

#include "fileIO/streams.h"
#include "fileIO/save-archive.h"

enum class eCharacterActionFunctionType {
    AWC_goBackFail,
    AWC_goBackFinish,

    AA_patrolFail,
    AA_patrolFinish,

    ArtA_buildFinish,
    ArtA_buildDelete,

    CTA_findTargetFinish,
    CTA_waitOutsideFinish,
    CTA_spreadFinish,

    CRA_collectFinish,
    CRA_collectFail,
    CRA_callCollectedActionFinish,

    FFA_lookForFireFail,
    FFA_putOutFireFinish,
    GA_lookForSoldierAttackFinish,
    GA_lookForRangeActionFinish,
    GA_teleportFinish,
    GA_hermesRunFinish,
    GA_spawnGodMissilePlaySound,

    GAA_loserDisappearFinish,
    GAA_fightFinish,
    GAA_destroyBuildingFinish,

    GMA_patrolFail,
    GMA_patrolFailFail,
    GMA_patrolFailFinish,
    GMA_spawnMultipleMissilesFinish,
    GMA_spawnMissileFinish,
    GMA_goToTargetFail,

    GRA_workOnDecisionFinish,
    GRA_workOnDecisionDeleteFail,

    killCharacterFinishFail,
    HA_patrolFail,
    HA_huntMonsterFinish,
    HA_fightMonsterDie,

    MA_destroyBuildingFinish,
    MA_lookForRangeActionFinishAttack,

    AHA_populateFinish,
    ApHA_healFinish,
    AtHA_giveFinish,
    HdHA_giveFinish,
    HfHA_provideFinish,
    HrHA_giveFinish,
    HmHA_provideFinish,
    PRHA_giveFinish,
    GAA_rangeAttackFinish,

    PA_patrolFail,
    PA_patrolFinish,

    SA_findHouseFail,
    SA_findHouseFinish,

    SA_collectDecisionFinish,
    SA_collectDecisionDeleteFail,
    SA_groomDecisionFinish,
    SA_groomDecisionDeleteFail,

    SA_goToFinish,
    SA_goHomeFinish,
    SA_waitAndGoHomeFinish,

    TA_tradeFail,
    TA_tradeFinish,
    TA_goToTradePostFinish,

    PT_spawnGetActorFinish,

    Char_fightFinish,
    Char_killWithCorpseFinish,

    SS_spawnFinish,

    ZHA_killFinish,

    RC_finishAction,
    RC_finishWalkingAction,

    HA_waitAndGoToHallFinish,

    GWA_huntMonsterFinish,
    GMinA_huntMonsterFinish,
    GMinA_huntSoldierFinish,
    GMinA_fightSoldierDie,

    enemyBoatFinish,

    PrSA_atAnimalFinish
};

using eCharActFuncType = eCharacterActionFunctionType;

class eCharacterActionFunction {
public:
    eCharacterActionFunction(GameBoard& board,
                             const eCharActFuncType t);
    virtual ~eCharacterActionFunction() = default;

    eCharActFuncType type() const { return mType; }
    GameBoard& board() { return mBoard; }

    void serialize(SaveArchive& ar) { serializeFields(ar); }

    virtual void call() = 0;

    static stdsptr<eCharacterActionFunction> sCreate(
            GameBoard& board,
            const eCharActFuncType type);
protected:
    virtual void serializeFields(SaveArchive& ar) { (void)ar; }
private:
    GameBoard& mBoard;
    const eCharActFuncType mType;
};

using eCharActFunc = eCharacterActionFunction;

#endif // CHARACTER_ACTION_FUNCTION_H

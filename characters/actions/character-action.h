#ifndef CHARACTER_ACTION_H
#define CHARACTER_ACTION_H

#include <functional>

#include "echaracteractiontype.h"
#include "pointers/estdselfref.h"
#include "pointers/estdpointer.h"
#include "character-action-function.h"

class eCharacter;
class eSaveArchive;

enum class eCityId;

enum class eCharacterActionState {
    running, finished, failed
};

// Save-stable. Values are persisted as raw ints. Never change an existing
// value. New entries must pick an unused number (typically max + 1).
enum class eCharActionType {
    animalAction          = 0,
    archerAction          = 1,
    artisanAction         = 2,
    buildAction           = 3,
    cartTransporterAction = 4,
    collectAction         = 5,
    collectResourceAction = 6,
    dieAction             = 7,
    fightAction           = 8,
    fireFighterAction     = 9,
    followAction          = 10,

    godAttackAction       = 11,
    godVisitAction        = 12,
    godWorshippedAction   = 13,

    aphroditeHelpAction   = 14,
    apolloHelpAction      = 15,
    aresHelpAction        = 16,
    artemisHelpAction     = 17,
    athenaHelpAction      = 18,
    atlasHelpAction       = 19,
    demeterHelpAction     = 20,
    dionysusHelpAction    = 21,
    hadesHelpAction       = 22,
    heraHelpAction        = 23,
    hephaestusHelpAction  = 24,
    hermesHelpAction      = 25,
    poseidonHelpAction    = 26,
    zeusHelpAction        = 27,

    godMonsterActionInd   = 28,

    growerAction          = 29,
    heroAction            = 30,
    takeCattleAction      = 31,
    replaceCattleAction   = 32,
    huntAction            = 33,
    monsterAction         = 34,
    moveAroundAction      = 35,
    movePathAction        = 36,
    moveToAction          = 37,
    patrolAction          = 38,
    patrolMoveAction      = 39,
    settlerAction         = 40,
    shepherdAction        = 41,
    soldierAction         = 42,
    traderAction          = 43,
    waitAction            = 44,
    sickDisgruntledAction = 45,
    defendCityAction      = 46,
    attackCityAction      = 47,

    dionysusFollowAction  = 48,

    triremeAction         = 49,
    wolfAction            = 50,
    deliverCartAction     = 51,
    getCartAction         = 52,
    vendorCartAction      = 53,
    storageDeliveryCartAction = 54,

    godMinionAction       = 55
};

class eCharacterAction : public eStdSelfRef {
public:
    eCharacterAction(eCharacter* const c,
                     const eCharActionType type);
    virtual ~eCharacterAction();

    virtual void increment(const int by) = 0;

    eCharacter* character() const { return mCharacter; }
    eCityId cityId() const;
    eCityId onCityId() const;
    eCharacterActionState state() const { return mState; }
    void setState(const eCharacterActionState state);

    eCharActionType type() const { return mType; }

    eGameBoard& board() const;

    void setFinishAction(const stdsptr<eCharActFunc>& f);
    void setFailAction(const stdsptr<eCharActFunc>& f);
    void setDeleteFailAction(const stdsptr<eCharActFunc>& d);
    eCharActFunc* finishAction() const { return mFinishAction.get(); }

    static stdsptr<eCharacterAction> sCreate(
            eCharacter* const c,
            const eCharActionType type);

    void serialize(eSaveArchive& ar);

    void setIOID(const int id) { mIOID = id; }
    int ioID() const { return mIOID; }

    eGameBoard& board();
protected:
    virtual void serializeFields(eSaveArchive& ar);
    virtual void resumeFromSavedState() {}

private:
    eGameBoard& mBoard;
    eCharacter* const mCharacter;
    int mIOID = -1;
    const eCharActionType mType;
    eCharacterActionState mState{eCharacterActionState::running};

    stdsptr<eCharActFunc> mFinishAction;
    stdsptr<eCharActFunc> mFailAction;
    stdsptr<eCharActFunc> mDeleteFailAction;
};

#endif // CHARACTER_ACTION_H

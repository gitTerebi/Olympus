#ifndef EZEUSHELPACTION_H
#define EZEUSHELPACTION_H

#include "characters/gods/actions/god-action.h"
#include "fileIO/esavearchive.h"

class eSaveArchive;

enum class eZeusHelpStage {
    none, appear, kill, killing, disappear
};

class eZeusHelpAction : public eGodAction {
public:
    eZeusHelpAction(eCharacter* const c);

    bool decide() override;

    static bool sHelpNeeded(const eCityId cid,
                            const eGameBoard& board);
protected:
    void serializeFields(eSaveArchive& ar) override;
    void resumeFromSavedState() override;
private:
    void kill();
    void spawnKillWait();

    eZeusHelpStage mStage{eZeusHelpStage::none};
};

class eZHA_killFinish : public eCharActFunc {
public:
    eZHA_killFinish(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::ZHA_killFinish) {}
    eZHA_killFinish(eGameBoard& board, const eCityId cid) :
        eCharActFunc(board, eCharActFuncType::ZHA_killFinish),
        mCid(cid) {}

    void call() override;

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.field("cityId", mCid);
    }
private:
    eCityId mCid;
};

#endif // EZEUSHELPACTION_H

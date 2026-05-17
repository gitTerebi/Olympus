#ifndef EZEUSHELPACTION_H
#define EZEUSHELPACTION_H

#include "characters/gods/actions/egodaction.h"
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

    void read(eReadStream& src) override {
        eSaveArchive ar(src);
        ar.field("cityId", mCid);
    }

    void write(eWriteStream& dst) const override {
        eSaveArchive ar(dst);
        ar.field("cityId", const_cast<eCityId&>(mCid));
    }
private:
    eCityId mCid;
};

#endif // EZEUSHELPACTION_H

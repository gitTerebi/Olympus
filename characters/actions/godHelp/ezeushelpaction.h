#ifndef EZEUSHELPACTION_H
#define EZEUSHELPACTION_H

#include "characters/actions/egodaction.h"

enum class eZeusHelpStage {
    none, appear, kill, disappear
};

class eZeusHelpAction : public eGodAction {
public:
    eZeusHelpAction(eCharacter* const c);

    bool decide() override;

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;

    static bool sHelpNeeded(const eCityId cid,
                            const eGameBoard& board);
private:
    void kill();

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
        src >> mCid;
    }

    void write(eWriteStream& dst) const override {
        dst << mCid;
    }
private:
    eCityId mCid;
};

#endif // EZEUSHELPACTION_H

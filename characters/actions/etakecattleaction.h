#ifndef ETAKECATTLEACTION_H
#define ETAKECATTLEACTION_H

#include "eactionwithcomeback.h"

class eCorral;
class eSaveArchive;

enum class eTakeCattleActionStage {
    none, get, goBack
};

class eTakeCattleAction : public eActionWithComeback {
public:
    eTakeCattleAction(eCharacter* const c,
                      eCorral* const cc = nullptr);

    bool decide() override;
    void increment(const int by) override;
protected:
    void serializeFields(eSaveArchive& ar) override;
    void resumeFromSavedState() override;
private:
    void goGetCattle();

    eTakeCattleActionStage mStage = eTakeCattleActionStage::none;
    stdptr<eCorral> mCorral;
    bool mNoCattle = false;
};

#endif // ETAKECATTLEACTION_H

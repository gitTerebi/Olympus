#ifndef EDIONYSUSFOLLOWACTION_H
#define EDIONYSUSFOLLOWACTION_H

#include "efollowaction.h"

enum class eCharacterType;
class eSaveArchive;

class eDionysusFollowAction : public eFollowAction {
public:
    eDionysusFollowAction(eCharacter* const f,
                          eCharacter* const c);
    eDionysusFollowAction(eCharacter* const c);

    void setFollower(eCharacter* const f);
    void setKillFollowerOnFinish();

    static bool sShouldFollow(const eCharacterType c);
protected:
    void serializeFields(eSaveArchive& ar) override;
    void resumeFromSavedState() override;
private:
    void increment(const int by) override;
    void attachKillCallback();

    stdptr<eCharacter> mFollower;
    bool mKillOnFinish = false;
};

#endif // EDIONYSUSFOLLOWACTION_H

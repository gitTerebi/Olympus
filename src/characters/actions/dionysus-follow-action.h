#ifndef DIONYSUS_FOLLOW_ACTION_H
#define DIONYSUS_FOLLOW_ACTION_H

#include "efollowaction.h"

enum class eCharacterType;
class SaveArchive;

class DionysusFollowAction : public eFollowAction {
public:
    DionysusFollowAction(eCharacter* const f,
                          eCharacter* const c);
    DionysusFollowAction(eCharacter* const c);

    void setFollower(eCharacter* const f);
    void setKillFollowerOnFinish();

    static bool sShouldFollow(const eCharacterType c);
protected:
    void serializeFields(SaveArchive& ar) override;
    void resumeFromSavedState() override;
private:
    void increment(const int by) override;
    void attachKillCallback();

    stdptr<eCharacter> mFollower;
    bool mKillOnFinish = false;
};

#endif // DIONYSUS_FOLLOW_ACTION_H

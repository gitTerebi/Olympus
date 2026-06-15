#ifndef EGODATTACKEVENT_H
#define EGODATTACKEVENT_H

#include "gameEvents/egameevent.h"

class eSanctuary;
class SaveArchive;
enum class GodType;

class eGodAttackEvent : public eGameEvent {
public:
    eGodAttackEvent(const eCityId cid,
                    const eGameEventBranch branch,
                    GameBoard& board);

    void setSanctuary(const stdptr<eSanctuary>& s);

    const std::vector<GodType>& types() const { return mTypes; }
    void setTypes(const std::vector<GodType>& types);

    void setRandom(const bool r);
    bool random() const { return mRandom; }

    void loadResources() const override;

    void trigger() override;
    std::string longName() const override;
protected:
    void serializeFields(SaveArchive& ar) override;
private:
    stdptr<eSanctuary> mSanctuary;
    std::vector<GodType> mTypes;
    int mNextId = 0;
    bool mRandom = false;
};


#endif // EGODATTACKEVENT_H

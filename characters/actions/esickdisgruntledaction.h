#ifndef ESICKDISGRUNTLEDACTION_H
#define ESICKDISGRUNTLEDACTION_H

#include "eactionwithcomeback.h"

class eSmallHouse;
class eSaveArchive;
class eJsonArchive;

class eSickDisgruntledAction : public eActionWithComeback {
public:
    eSickDisgruntledAction(eCharacter* const c,
                           eSmallHouse* const ch);

    bool decide() override;

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;
    void serializeJson(eJsonArchive& ar) override;
protected:
    void patrol();
    void goBackDecision(const stdsptr<eWalkableObject>& w =
                            eWalkableObject::sCreateRoadAvenue());
private:
    void serialize(eSaveArchive& ar);

    eSmallHouse* mBuilding = nullptr;

    bool mGoBackNext = false;
};

#endif // ESICKDISGRUNTLEDACTION_H

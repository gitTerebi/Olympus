#ifndef EHEPHAESTUSHELPACTION_H
#define EHEPHAESTUSHELPACTION_H

#include "characters/gods/actions/egodaction.h"

class eSaveArchive;

enum class eHephaestusHelpStage {
    none, appear, provide, disappear
};

class eHephaestusHelpAction : public eGodAction {
public:
    eHephaestusHelpAction(eCharacter* const c);

    bool decide() override;

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;
    void serializeJson(eJsonArchive& ar) override;

    static bool sHelpNeeded(const eCityId cid,
                            const eGameBoard& board);
private:
    void serialize(eSaveArchive& ar);
    void provide();

    eHephaestusHelpStage mStage{eHephaestusHelpStage::none};
};

#endif // EHEPHAESTUSHELPACTION_H

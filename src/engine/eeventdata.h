#ifndef EEVENTDATA_H
#define EEVENTDATA_H

#include "edate.h"
#include "eresourcetype.h"
#include "emessageeventtype.h"
#include "ecityid.h"
#include "e-city-attitude.h"

enum class GodType;
enum class eHeroType;
enum class GodQuestId;
enum class eMonsterType;
enum class eCityId;

struct eEventData {
    eEventData() : fTarget(ePlayerCityTarget()) {}
    eEventData(const ePlayerCityTarget& target) :
        fTarget(target) {}

    ePlayerCityTarget fTarget;
    eMessageEventType fType = eMessageEventType::common;
    eDate fDate = eDate(1, eMonth::january, -1500);
    std::string fPlayerName;
    eTile* fTile = nullptr;
    stdptr<eCharacter> fChar;
    int fBribe = 0;
    int fEventRuntimeId = -1;
    int fCloseResponse = -1;
    int fPrimaryResponse = -1;
    std::map<eCityId, std::string> fCityNames;
    std::map<eCityId, int> fCityConditionalResponses;
    int fSecondaryResponse = -1;
    int fTertiaryResponse = -1;
    stdsptr<WorldCity> fCity;
    stdsptr<WorldCity> fRivalCity;
    eResourceType fResourceType = eResourceType::drachmas;
    int fResourceCount = 0;
    std::map<eCityId, int> fCSpaceCount;
    int fTime = 0;
    std::string fReason;
    GodType fGod = static_cast<GodType>(0);
    GodQuestId fQuestId = static_cast<GodQuestId>(0);
    eHeroType fHero = static_cast<eHeroType>(0);
    eMonsterType fMonster = static_cast<eMonsterType>(0);
    eCityAttitudeMessage fCityAttitudeMessage = eCityAttitudeMessage::none;
};

#endif // EEVENTDATA_H

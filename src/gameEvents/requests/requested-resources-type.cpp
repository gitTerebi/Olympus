#include "requested-resources-type.h"

#include "messages.h"

struct eReceiveRequestEntry {
    eEvent fInitial;
    eEvent fReminder;
    eEvent fOverdue;
    eEvent fWarning;
    eEvent fTooLate;
    eEvent fComply;
    eEvent fRefuse;
    const RequestedResourcesMessages* fMessages;
};

#define DEFINE_ENTRY(enumPrefix, fieldPrefix, relation, msgsSuffix) \
    static const eReceiveRequestEntry k##fieldPrefix##relation = { \
        eEvent::enumPrefix##relation##Initial, \
        eEvent::enumPrefix##relation##Reminder, \
        eEvent::enumPrefix##relation##Overdue, \
        eEvent::enumPrefix##relation##Warning, \
        eEvent::enumPrefix##relation##TooLate, \
        eEvent::enumPrefix##relation##Comply, \
        eEvent::enumPrefix##relation##Refuse, \
        &Messages::instance.f##fieldPrefix##msgsSuffix \
    }

DEFINE_ENTRY(generalRequest, GeneralRequest, Ally, AllyS);
DEFINE_ENTRY(generalRequest, GeneralRequest, Rival, RivalD);
DEFINE_ENTRY(generalRequest, GeneralRequest, Subject, SubjectP);
DEFINE_ENTRY(generalRequest, GeneralRequest, Parent, ParentR);
DEFINE_ENTRY(famine, Famine, Ally, AllyS);
DEFINE_ENTRY(famine, Famine, Rival, RivalD);
DEFINE_ENTRY(famine, Famine, Subject, SubjectP);
DEFINE_ENTRY(famine, Famine, Parent, ParentR);
DEFINE_ENTRY(project, Project, Ally, AllyS);
DEFINE_ENTRY(project, Project, Rival, RivalD);
DEFINE_ENTRY(project, Project, Subject, SubjectP);
DEFINE_ENTRY(project, Project, Parent, ParentR);
DEFINE_ENTRY(festival, Festival, Ally, AllyS);
DEFINE_ENTRY(festival, Festival, Rival, RivalD);
DEFINE_ENTRY(festival, Festival, Subject, SubjectP);
DEFINE_ENTRY(festival, Festival, Parent, ParentR);
DEFINE_ENTRY(financialWoes, FinancialWoes, Ally, AllyS);
DEFINE_ENTRY(financialWoes, FinancialWoes, Rival, RivalD);
DEFINE_ENTRY(financialWoes, FinancialWoes, Subject, SubjectP);
DEFINE_ENTRY(financialWoes, FinancialWoes, Parent, ParentR);

#undef DEFINE_ENTRY

static const eReceiveRequestEntry& getEntry(
    const RequestedResourcesType type,
    const WorldCity& city) {
    if(city.isRival()) {
        switch(type) {
        case RequestedResourcesType::general: return kGeneralRequestRival;
        case RequestedResourcesType::famine: return kFamineRival;
        case RequestedResourcesType::project: return kProjectRival;
        case RequestedResourcesType::festival: return kFestivalRival;
        case RequestedResourcesType::financialWoes: return kFinancialWoesRival;
        default: break;
        }
    } else if(city.isVassal() || city.isColony()) {
        switch(type) {
        case RequestedResourcesType::general: return kGeneralRequestSubject;
        case RequestedResourcesType::famine: return kFamineSubject;
        case RequestedResourcesType::project: return kProjectSubject;
        case RequestedResourcesType::festival: return kFestivalSubject;
        case RequestedResourcesType::financialWoes: return kFinancialWoesSubject;
        default: break;
        }
    } else if(city.isParentCity()) {
        switch(type) {
        case RequestedResourcesType::general: return kGeneralRequestParent;
        case RequestedResourcesType::famine: return kFamineParent;
        case RequestedResourcesType::project: return kProjectParent;
        case RequestedResourcesType::festival: return kFestivalParent;
        case RequestedResourcesType::financialWoes: return kFinancialWoesParent;
        default: break;
        }
    } else {
        switch(type) {
        case RequestedResourcesType::general: return kGeneralRequestAlly;
        case RequestedResourcesType::famine: return kFamineAlly;
        case RequestedResourcesType::project: return kProjectAlly;
        case RequestedResourcesType::festival: return kFestivalAlly;
        case RequestedResourcesType::financialWoes: return kFinancialWoesAlly;
        default: break;
        }
    }
    // Tribute fallback
    static eReceiveRequestEntry tributeEntry = {
        eEvent::generalRequestTributeInitial,
        eEvent::generalRequestTributeReminder,
        eEvent::generalRequestTributeOverdue,
        eEvent::generalRequestTributeWarning,
        eEvent::generalRequestTributeTooLate,
        eEvent::generalRequestTributeComply,
        eEvent::generalRequestTributeRefuse,
        &Messages::instance.fTributeRequest
    };
    return tributeEntry;
}

eEvent requestedResourcesFinishEvent(
    const RequestedResourcesType type,
    const WorldCity& city,
    const RequestedResourcesFinish finish) {
    const auto& entry = getEntry(type, city);
    switch(finish) {
    case RequestedResourcesFinish::tooLate: return entry.fTooLate;
    case RequestedResourcesFinish::comply: return entry.fComply;
    case RequestedResourcesFinish::refuse: return entry.fRefuse;
    }
    return eEvent::fire; // invalid
}

eEvent requestedResourcesStepEvent(
    const RequestedResourcesType type,
    const WorldCity& city,
    const int requestStep) {
    const auto& entry = getEntry(type, city);
    switch(requestStep) {
    case 0: return entry.fInitial;
    case 1: return entry.fReminder;
    case 2: return entry.fOverdue;
    case 3: return entry.fWarning;
    case 4: return entry.fTooLate;
    default: return entry.fInitial;
    }
}

const RequestedResourcesMessages* requestedResourcesMessages(
    const RequestedResourcesType type,
    const WorldCity& city) {
    return getEntry(type, city).fMessages;
}

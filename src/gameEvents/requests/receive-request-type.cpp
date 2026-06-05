#include "receive-request-type.h"

#include "emessages.h"

struct eReceiveRequestEntry {
    eEvent fInitial;
    eEvent fReminder;
    eEvent fOverdue;
    eEvent fWarning;
    eEvent fTooLate;
    eEvent fComply;
    eEvent fRefuse;
    const eReceiveRequestMessages* fMessages;
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
        &eMessages::instance.f##fieldPrefix##msgsSuffix \
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
    const ReceiveRequestType type,
    const WorldCity& city) {
    if(city.isRival()) {
        switch(type) {
        case ReceiveRequestType::general: return kGeneralRequestRival;
        case ReceiveRequestType::famine: return kFamineRival;
        case ReceiveRequestType::project: return kProjectRival;
        case ReceiveRequestType::festival: return kFestivalRival;
        case ReceiveRequestType::financialWoes: return kFinancialWoesRival;
        default: break;
        }
    } else if(city.isVassal() || city.isColony()) {
        switch(type) {
        case ReceiveRequestType::general: return kGeneralRequestSubject;
        case ReceiveRequestType::famine: return kFamineSubject;
        case ReceiveRequestType::project: return kProjectSubject;
        case ReceiveRequestType::festival: return kFestivalSubject;
        case ReceiveRequestType::financialWoes: return kFinancialWoesSubject;
        default: break;
        }
    } else if(city.isParentCity()) {
        switch(type) {
        case ReceiveRequestType::general: return kGeneralRequestParent;
        case ReceiveRequestType::famine: return kFamineParent;
        case ReceiveRequestType::project: return kProjectParent;
        case ReceiveRequestType::festival: return kFestivalParent;
        case ReceiveRequestType::financialWoes: return kFinancialWoesParent;
        default: break;
        }
    } else {
        switch(type) {
        case ReceiveRequestType::general: return kGeneralRequestAlly;
        case ReceiveRequestType::famine: return kFamineAlly;
        case ReceiveRequestType::project: return kProjectAlly;
        case ReceiveRequestType::festival: return kFestivalAlly;
        case ReceiveRequestType::financialWoes: return kFinancialWoesAlly;
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
        &eMessages::instance.fTributeRequest
    };
    return tributeEntry;
}

eEvent receiveRequestFinishEvent(
    const ReceiveRequestType type,
    const WorldCity& city,
    const ReceiveRequestFinish finish) {
    const auto& entry = getEntry(type, city);
    switch(finish) {
    case ReceiveRequestFinish::tooLate: return entry.fTooLate;
    case ReceiveRequestFinish::comply: return entry.fComply;
    case ReceiveRequestFinish::refuse: return entry.fRefuse;
    }
    return eEvent::fire; // invalid
}

eEvent receiveRequestStepEvent(
    const ReceiveRequestType type,
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

const eReceiveRequestMessages* receiveRequestMessages(
    const ReceiveRequestType type,
    const WorldCity& city) {
    return getEntry(type, city).fMessages;
}

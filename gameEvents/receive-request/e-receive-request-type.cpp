#include "e-receive-request-type.h"

#include "emessages.h"

struct eReceiveRequestEntry {
    eEvent fTooLate;
    eEvent fComply;
    eEvent fRefuse;
    eEvent fInitial;
    eEvent fReminder;
    eEvent fOverdue;
    eEvent fWarning;
    const eReceiveRequestMessages* fMessages;
};

#define DEFINE_ENTRY(enumPrefix, fieldPrefix, relation, msgsSuffix) \
    static const eReceiveRequestEntry k##fieldPrefix##relation = { \
        eEvent::enumPrefix##relation##TooLate, \
        eEvent::enumPrefix##relation##Comply, \
        eEvent::enumPrefix##relation##Refuse, \
        eEvent::enumPrefix##relation##Initial, \
        eEvent::enumPrefix##relation##Reminder, \
        eEvent::enumPrefix##relation##Overdue, \
        eEvent::enumPrefix##relation##Warning, \
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
    const eReceiveRequestType type,
    const eWorldCity& city) {
    if(city.isRival()) {
        switch(type) {
        case eReceiveRequestType::general: return kGeneralRequestRival;
        case eReceiveRequestType::famine: return kFamineRival;
        case eReceiveRequestType::project: return kProjectRival;
        case eReceiveRequestType::festival: return kFestivalRival;
        case eReceiveRequestType::financialWoes: return kFinancialWoesRival;
        default: break;
        }
    } else if(city.isVassal() || city.isColony()) {
        switch(type) {
        case eReceiveRequestType::general: return kGeneralRequestSubject;
        case eReceiveRequestType::famine: return kFamineSubject;
        case eReceiveRequestType::project: return kProjectSubject;
        case eReceiveRequestType::festival: return kFestivalSubject;
        case eReceiveRequestType::financialWoes: return kFinancialWoesSubject;
        default: break;
        }
    } else if(city.isParentCity()) {
        switch(type) {
        case eReceiveRequestType::general: return kGeneralRequestParent;
        case eReceiveRequestType::famine: return kFamineParent;
        case eReceiveRequestType::project: return kProjectParent;
        case eReceiveRequestType::festival: return kFestivalParent;
        case eReceiveRequestType::financialWoes: return kFinancialWoesParent;
        default: break;
        }
    } else {
        switch(type) {
        case eReceiveRequestType::general: return kGeneralRequestAlly;
        case eReceiveRequestType::famine: return kFamineAlly;
        case eReceiveRequestType::project: return kProjectAlly;
        case eReceiveRequestType::festival: return kFestivalAlly;
        case eReceiveRequestType::financialWoes: return kFinancialWoesAlly;
        default: break;
        }
    }
    // Tribute fallback
    static eReceiveRequestEntry tributeEntry = {
        eEvent::generalRequestTributeTooLate,
        eEvent::generalRequestTributeComply,
        eEvent::generalRequestTributeRefuse,
        eEvent::generalRequestTributeInitial,
        eEvent::generalRequestTributeReminder,
        eEvent::generalRequestTributeOverdue,
        eEvent::generalRequestTributeWarning,
        &eMessages::instance.fTributeRequest
    };
    return tributeEntry;
}

eEvent receiveRequestFinishEvent(
    const eReceiveRequestType type,
    const eWorldCity& city,
    const eReceiveRequestFinish finish) {
    const auto& entry = getEntry(type, city);
    switch(finish) {
    case eReceiveRequestFinish::tooLate: return entry.fTooLate;
    case eReceiveRequestFinish::comply: return entry.fComply;
    case eReceiveRequestFinish::refuse: return entry.fRefuse;
    }
    return eEvent::fire; // invalid
}

eEvent receiveRequestStepEvent(
    const eReceiveRequestType type,
    const eWorldCity& city,
    const int requestStep) {
    const auto& entry = getEntry(type, city);
    switch(requestStep) {
    case 0: return entry.fInitial;
    case 1: return entry.fReminder;
    case 2: return entry.fOverdue;
    case 3: return entry.fWarning;
    case 4: return entry.fOverdue;
    default: return entry.fInitial;
    }
}

const eReceiveRequestMessages* receiveRequestMessages(
    const eReceiveRequestType type,
    const eWorldCity& city) {
    return getEntry(type, city).fMessages;
}

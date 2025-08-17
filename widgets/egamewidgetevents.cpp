#include "egamewidget.h"

#include "audio/esounds.h"
#include "emessages.h"
#include "engine/eevent.h"
#include "estringhelpers.h"
#include "engine/egameboard.h"

void eGameWidget::handleGodQuestEvent(eEventData& ed,
                                      const bool fulfilled) {
    const auto hero = ed.fHero;
    const auto id = ed.fQuestId;
    const auto god = ed.fGod;
    if(fulfilled) {
        eSounds::playGodSound(god, eGodSound::questFinished);
    } else {
        eSounds::playGodSound(god, eGodSound::quest);
    }
    const auto& inst = eMessages::instance;
    const auto gm = inst.godMessages(god);
    if(!gm) return;
    const eQuestMessages* qm = nullptr;
    switch(id) {
    case eGodQuestId::godQuest1:
        qm = &gm->fQuest1;
        break;
    case eGodQuestId::godQuest2:
        qm = &gm->fQuest2;
        break;
    }
    const eEventMessageType* emt = nullptr;
    if(fulfilled) {
        emt = &qm->fFulfilled;
    } else {
        emt = &qm->fQuest;
    }
    auto msg = emt->fFull;
    const auto hm = eHero::sHeroName(hero);
    eStringHelpers::replaceAll(msg.fTitle, "[hero_needed]", hm);
    eStringHelpers::replaceAll(msg.fText, "[hero_needed]", hm);
    showMessage(ed, msg);
}

void eGameWidget::handleGodVisitEvent(eEventData& ed) {
    const auto god = ed.fGod;
    const auto& inst = eMessages::instance;
    const auto msgs = inst.godMessages(god);
    int& lm = msgs->fLastMessage;
    lm = lm > 2 ? 0 : (lm + 1);
    if(lm == 0) {
        eSounds::playGodSound(god, eGodSound::wooing0);
        showMessage(ed, msgs->fWooing0);
    } else if(lm == 1) {
        eSounds::playGodSound(god, eGodSound::jealousy1);
        showMessage(ed, msgs->fJealousy1);
    } else if(lm == 2) {
        eSounds::playGodSound(god, eGodSound::jealousy2);
        showMessage(ed, msgs->fJealousy2);
    }
}

void eGameWidget::handleGodInvasionEvent(eEventData& ed) {
    const auto& inst = eMessages::instance;
    const auto msgs = inst.godMessages(ed.fGod);
    showMessage(ed, msgs->fInvades);
    eSounds::playGodSound(ed.fGod, eGodSound::invade);
}

void eGameWidget::handleGodHelpEvent(eEventData& ed) {
    const auto& inst = eMessages::instance;
    const auto msgs = inst.godMessages(ed.fGod);
    showMessage(ed, msgs->fHelps);
    eSounds::playGodSound(ed.fGod, eGodSound::help);
}

void eGameWidget::handleSanctuaryComplete(eEventData& ed) {
    const auto& inst = eMessages::instance;
    const auto gm = inst.godMessages(ed.fGod);
    if(!gm) return;
    showMessage(ed, gm->fSanctuaryComplete);
}

void eGameWidget::handleMonsterUnleashEvent(eEventData& ed) {
    const auto& inst = eMessages::instance;
    const auto gm = inst.godMessages(ed.fGod);
    if(!gm) return;
    eSounds::playGodSound(ed.fGod, eGodSound::monster);
    showMessage(ed, gm->fMonster, true);
}

void eGameWidget::handleMonsterInCityEvent(eEventData& ed) {
    const auto& inst = eMessages::instance;
    const auto gm = inst.monsterMessages(ed.fMonster);
    if(!gm) return;
    eSounds::playMonsterSound(ed.fMonster, eMonsterSound::voice);
    showMessage(ed, gm->fInCity, true);
}

void eGameWidget::handleMonsterInvasion36Event(eEventData& ed) {
    const auto& inst = eMessages::instance;
    const auto gm = inst.monsterMessages(ed.fMonster);
    if(!gm) return;
    eMessageType msg = gm->fInvasion36;
    std::string reason = ed.fReason;
    if(reason.empty()) {
        reason = gm->fMonsterAttackReason;
    }
    eStringHelpers::replace(msg.fFull.fText, "[reason_phrase]", reason);
    eStringHelpers::replace(msg.fFull.fText, "[time_until_attack]", "36");
    eStringHelpers::replace(msg.fCondensed.fText, "[time_until_attack]", "36");
    showMessage(ed, msg);
}

void eGameWidget::handleMonsterInvasion24Event(eEventData& ed) {
    const auto& inst = eMessages::instance;
    const auto gm = inst.monsterMessages(ed.fMonster);
    if(!gm) return;
    showMessage(ed, gm->fInvasion24);
}

void eGameWidget::handleMonsterInvasion12Event(eEventData& ed) {
    const auto& inst = eMessages::instance;
    const auto gm = inst.monsterMessages(ed.fMonster);
    if(!gm) return;
    showMessage(ed, gm->fInvasion12);
}

void eGameWidget::handleMonsterInvasion6Event(eEventData& ed) {
    const auto& inst = eMessages::instance;
    const auto gm = inst.monsterMessages(ed.fMonster);
    if(!gm) return;
    showMessage(ed, gm->fInvasion6);
}

void eGameWidget::handleMonsterInvasion1Event(eEventData& ed) {
    const auto& inst = eMessages::instance;
    const auto gm = inst.monsterMessages(ed.fMonster);
    if(!gm) return;
    showMessage(ed, gm->fInvasion1);
}

void eGameWidget::handleMonsterInvasionEvent(eEventData& ed) {
    const auto& inst = eMessages::instance;
    const auto gm = inst.monsterMessages(ed.fMonster);
    if(!gm) return;
    eSounds::playMonsterSound(ed.fMonster, eMonsterSound::voice);
    showMessage(ed, gm->fInvasion, true);
}

void eGameWidget::handleMonsterSlainEvent(eEventData& ed) {
    const auto& inst = eMessages::instance;
    const auto gm = inst.monsterMessages(ed.fMonster);
    if(!gm) return;
    showMessage(ed, gm->fSlain);
}

void eGameWidget::handleHeroArrivalEvent(eEventData& ed) {
    const auto& inst = eMessages::instance;
    const auto gm = inst.heroMessages(ed.fHero);
    if(!gm) return;
    eSounds::playHeroSound(ed.fHero, eHeroSound::arrived);
    showMessage(ed, gm->fArrival);
}

void eGameWidget::handleEvent(const eEvent e, eEventData& ed) {
    const auto& target = ed.fTarget;
    const auto ppid = mBoard->personPlayer();
    if(target.isPlayerTarget()) {
        const auto pid = target.playerTarget();
        if(pid != ppid) return;
    } else if(target.isCityTarget()) {
        const auto cid = target.cityTarget();
        const auto pid = mBoard->cityIdToPlayerId(cid);
        if(pid != ppid) return;
    }
    const auto& inst = eMessages::instance;
    switch(e) {
    case eEvent::fire: {
        eSounds::playFireSound();
        showMessage(ed, inst.fFire);
    } break;
    case eEvent::collapse: {
        eSounds::playCollapseSound();
        showMessage(ed, inst.fCollapse);
    } break;

    case eEvent::godVisit:
        handleGodVisitEvent(ed);
        break;
    case eEvent::godInvasion:
        handleGodInvasionEvent(ed);
        break;
    case eEvent::godHelp:
        handleGodHelpEvent(ed);
        break;

    case eEvent::godMonsterUnleash:
        handleMonsterUnleashEvent(ed);
        break;

    case eEvent::monsterInCity:
        handleMonsterInCityEvent(ed);
        break;

    case eEvent::monsterInvasion36:
        handleMonsterInvasion36Event(ed);
        return;
    case eEvent::monsterInvasion24:
        handleMonsterInvasion24Event(ed);
        return;
    case eEvent::monsterInvasion12:
        handleMonsterInvasion12Event(ed);
        return;
    case eEvent::monsterInvasion6:
        handleMonsterInvasion6Event(ed);
        return;
    case eEvent::monsterInvasion1:
        handleMonsterInvasion1Event(ed);
        return;
    case eEvent::monsterInvasion:
        handleMonsterInvasionEvent(ed);
        break;
    case eEvent::monsterSlain:
        handleMonsterSlainEvent(ed);
        return;

    case eEvent::godQuest:
    case eEvent::godQuestFulfilled: {
        handleGodQuestEvent(ed, e == eEvent::godQuestFulfilled);
        return;
    } break;
    case eEvent::sanctuaryComplete: {
        handleSanctuaryComplete(ed);
        return;
    } break;

    case eEvent::heroArrival:
        handleHeroArrivalEvent(ed);
        break;

    case eEvent::invasion36: {
        showMessage(ed, eMessages::invasionMessage(inst.fInvasion36, ed.fReason, 36));
    } break;
    case eEvent::invasion24: {
        showMessage(ed, eMessages::invasionMessage(inst.fInvasion24, ed.fReason, 24));
    } break;
    case eEvent::invasion12: {
        showMessage(ed, eMessages::invasionMessage(inst.fInvasion12, ed.fReason, 12));
    } break;
    case eEvent::invasion6: {
        showMessage(ed, eMessages::invasionMessage(inst.fInvasion6, ed.fReason, 6));
    } break;
    case eEvent::invasion1: {
        showMessage(ed, eMessages::invasionMessage(inst.fInvasion1, ed.fReason, 1));
    } break;
    case eEvent::invasion: {
        showMessage(ed, eMessages::invasionMessage(inst.fInvasion, ed.fReason, 0));
    } break;
    case eEvent::invasionBribed: {
        showMessage(ed, inst.fInvasionBribed);
        return;
    } break;
    case eEvent::invasionDefeat: {
        showMessage(ed, inst.fInvasionDefeat);
        return;
    } break;
    case eEvent::invasionVictoryMonn: {
        showMessage(ed, inst.fInvasionVictoryMonument);
        return;
    } break;
    case eEvent::invasionVictory: {
        showMessage(ed, inst.fInvasionVictory);
        return;
    } break;
    case eEvent::tributePaid: {
        showMessage(ed, inst.fTributePaidGranted);
        return;
    } break;
    case eEvent::tributeAccepted: {
        showMessage(ed, inst.fTributePaidAccepted, true);
        return;
    } break;
    case eEvent::tributeDeclined: {
        showMessage(ed, inst.fTributePaidRefused, true);
        return;
    } break;
    case eEvent::tributePostponed: {
        showMessage(ed, inst.fTributePostponed, true);
        return;
    } break;
    case eEvent::requestGranted: {
        showMessage(ed, inst.fRequestGranted);
        return;
    } break;
    case eEvent::requestPartialSpace: {
        showMessage(ed, inst.fRequestPartialSpace);
        return;
    } break;
    case eEvent::requestInsufficientSpace: {
        showMessage(ed, inst.fRequestInsufficientSpace);
        return;
    } break;
    case eEvent::requestLastChance: {
        showMessage(ed, inst.fRequestLastChance);
        return;
    } break;
    case eEvent::requestAccepted: {
        showMessage(ed, inst.fRequestAccepted, true);
        return;
    } break;
    case eEvent::requestPostponed: {
        showMessage(ed, inst.fRequestPostponed, true);
        return;
    } break;
    case eEvent::requestRefused: {
        showMessage(ed, inst.fRequestRefused, true);
        return;
    } break;
    case eEvent::requestForfeited: {
        showMessage(ed, inst.fRequestForfeited, true);
        return;
    } break;

    case eEvent::giftGranted: {
        showMessage(ed, inst.fGiftGranted.fGranted);
        return;
    } break;
    case eEvent::giftPartialSpace: {
        showMessage(ed, inst.fGiftGranted.fPartialSpace);
        return;
    } break;
    case eEvent::giftInsufficientSpace: {
        showMessage(ed, inst.fGiftGranted.fInsufficientSpace);
        return;
    } break;
    case eEvent::giftLastChance: {
        showMessage(ed, inst.fGiftGranted.fLastChance);
        return;
    } break;
    case eEvent::giftAccepted: {
        showMessage(ed, inst.fGiftGranted.fAccepted, true);
        return;
    } break;
    case eEvent::giftPostponed: {
        showMessage(ed, inst.fGiftGranted.fPostponed, true);
        return;
    } break;
    case eEvent::giftRefused: {
        showMessage(ed, inst.fGiftGranted.fRefused, true);
        return;
    } break;
    case eEvent::giftForfeited: {
        showMessage(ed, inst.fGiftGranted.fForfeited, true);
        return;
    } break;
    case eEvent::giftCashGranted: {
        showMessage(ed, inst.fGiftGranted.fCashGranted, true);
        return;
    } break;
    case eEvent::giftCashAccepted: {
        showMessage(ed, inst.fGiftGranted.fCashAccepted, true);
        return;
    } break;

    case eEvent::demandRefused: {
        showMessage(ed, inst.fDemandRefused, true);
        return;
    } break;
    case eEvent::giftReceivedDrachmas: {
        showMessage(ed, inst.fGiftReceivedDrachmas, true);
        return;
    } break;
    case eEvent::giftReceivedNeeded: {
        showMessage(ed, inst.fGiftReceivedNeeded, true);
        return;
    } break;
    case eEvent::giftReceivedSells: {
        showMessage(ed, inst.fGiftReceivedSells, true);
        return;
    } break;
    case eEvent::giftReceivedNotNeeded: {
        showMessage(ed, inst.fGiftReceivedNotNeeded, true);
        return;
    } break;
    case eEvent::giftReceivedRefuse: {
        showMessage(ed, inst.fGiftReceivedRefuse, true);
        return;
    } break;

    case eEvent::generalRequestAllyInitial: {
        showMessage(ed, inst.fGeneralRequestAllyS.fInitial, true);
        return;
    } break;
    case eEvent::generalRequestAllyReminder: {
        showMessage(ed, inst.fGeneralRequestAllyS.fReminder, true);
        return;
    } break;
    case eEvent::generalRequestAllyOverdue: {
        showMessage(ed, inst.fGeneralRequestAllyS.fOverdue, true);
        return;
    } break;
    case eEvent::generalRequestAllyWarning: {
        showMessage(ed, inst.fGeneralRequestAllyS.fWarning, true);
        return;
    } break;
    case eEvent::generalRequestAllyComply: {
        const auto m = eMessages::favorMessage(
                    inst.fGeneralRequestAllyS.fComply);
        showMessage(ed, m, true);
        return;
    } break;
    case eEvent::generalRequestAllyTooLate: {
        const auto m = eMessages::dfavorMessage(
                    inst.fGeneralRequestAllyS.fTooLate);
        showMessage(ed, m, true);
        return;
    } break;
    case eEvent::generalRequestAllyRefuse: {
        const auto m = eMessages::dfavorMessage(
                           inst.fGeneralRequestAllyS.fRefuse);
        showMessage(ed, m, true);
        return;
    } break;

    case eEvent::generalRequestRivalInitial: {
        showMessage(ed, inst.fGeneralRequestRivalD.fInitial, true);
        return;
    } break;
    case eEvent::generalRequestRivalReminder: {
        showMessage(ed, inst.fGeneralRequestRivalD.fReminder, true);
        return;
    } break;
    case eEvent::generalRequestRivalOverdue: {
        showMessage(ed, inst.fGeneralRequestRivalD.fOverdue, true);
        return;
    } break;
    case eEvent::generalRequestRivalWarning: {
        showMessage(ed, inst.fGeneralRequestRivalD.fWarning, true);
        return;
    } break;
    case eEvent::generalRequestRivalComply: {
        const auto m = eMessages::favorMessage(
                    inst.fGeneralRequestRivalD.fComply);
        showMessage(ed, m, true);
        return;
    } break;
    case eEvent::generalRequestRivalTooLate: {
        const auto m = eMessages::dfavorMessage(
                    inst.fGeneralRequestRivalD.fTooLate);
        showMessage(ed, m, true);
        return;
    } break;
    case eEvent::generalRequestRivalRefuse: {
        const auto m = eMessages::dfavorMessage(
                           inst.fGeneralRequestRivalD.fRefuse);
        showMessage(ed, m, true);
        return;
    } break;

    case eEvent::generalRequestSubjectInitial: {
        showMessage(ed, inst.fGeneralRequestSubjectP.fInitial, true);
        return;
    } break;
    case eEvent::generalRequestSubjectReminder: {
        showMessage(ed, inst.fGeneralRequestSubjectP.fReminder, true);
        return;
    } break;
    case eEvent::generalRequestSubjectOverdue: {
        showMessage(ed, inst.fGeneralRequestSubjectP.fOverdue, true);
        return;
    } break;
    case eEvent::generalRequestSubjectWarning: {
        showMessage(ed, inst.fGeneralRequestSubjectP.fWarning, true);
        return;
    } break;
    case eEvent::generalRequestSubjectComply: {
        const auto m = eMessages::favorMessage(
                    inst.fGeneralRequestSubjectP.fComply);
        showMessage(ed, m, true);
        return;
    } break;
    case eEvent::generalRequestSubjectTooLate: {
        const auto m = eMessages::dfavorMessage(
                    inst.fGeneralRequestSubjectP.fTooLate);
        showMessage(ed, m, true);
        return;
    } break;
    case eEvent::generalRequestSubjectRefuse: {
        const auto m = eMessages::dfavorMessage(
                           inst.fGeneralRequestSubjectP.fRefuse);
        showMessage(ed, m, true);
        return;
    } break;

    case eEvent::generalRequestParentInitial: {
        showMessage(ed, inst.fGeneralRequestParentR.fInitial, true);
        return;
    } break;
    case eEvent::generalRequestParentReminder: {
        showMessage(ed, inst.fGeneralRequestParentR.fReminder, true);
        return;
    } break;
    case eEvent::generalRequestParentOverdue: {
        showMessage(ed, inst.fGeneralRequestParentR.fOverdue, true);
        return;
    } break;
    case eEvent::generalRequestParentWarning: {
        showMessage(ed, inst.fGeneralRequestParentR.fWarning, true);
        return;
    } break;
    case eEvent::generalRequestParentComply: {
        const auto m = eMessages::favorMessage(
                    inst.fGeneralRequestParentR.fComply);
        showMessage(ed, m, true);
        return;
    } break;
    case eEvent::generalRequestParentTooLate: {
        const auto m = eMessages::dfavorMessage(
                    inst.fGeneralRequestParentR.fTooLate);
        showMessage(ed, m, true);
        return;
    } break;
    case eEvent::generalRequestParentRefuse: {
        const auto m = eMessages::dfavorMessage(
                           inst.fGeneralRequestParentR.fRefuse);
        showMessage(ed, m, true);
        return;
    } break;

    case eEvent::generalRequestTributeInitial: {
        showMessage(ed, inst.fTributeRequest.fInitial, true);
        return;
    } break;
    case eEvent::generalRequestTributeReminder: {
        showMessage(ed, inst.fTributeRequest.fReminder, true);
        return;
    } break;
    case eEvent::generalRequestTributeOverdue: {
        showMessage(ed, inst.fTributeRequest.fOverdue, true);
        return;
    } break;
    case eEvent::generalRequestTributeWarning: {
        showMessage(ed, inst.fTributeRequest.fWarning, true);
        return;
    } break;
    case eEvent::generalRequestTributeComply: {
        const auto m = eMessages::favorMessage(
                    inst.fTributeRequest.fComply);
        showMessage(ed, m, true);
        return;
    } break;
    case eEvent::generalRequestTributeTooLate: {
        const auto m = eMessages::dfavorMessage(
                    inst.fTributeRequest.fTooLate);
        showMessage(ed, m, true);
        return;
    } break;
    case eEvent::generalRequestTributeRefuse: {
        const auto m = eMessages::dfavorMessage(
                           inst.fTributeRequest.fRefuse);
        showMessage(ed, m, true);
        return;
    } break;

    case eEvent::famineAllyInitial: {
        showMessage(ed, inst.fFamineAllyS.fInitial, true);
        return;
    } break;
    case eEvent::famineAllyReminder: {
        showMessage(ed, inst.fFamineAllyS.fReminder, true);
        return;
    } break;
    case eEvent::famineAllyOverdue: {
        showMessage(ed, inst.fFamineAllyS.fOverdue, true);
        return;
    } break;
    case eEvent::famineAllyWarning: {
        showMessage(ed, inst.fFamineAllyS.fWarning, true);
        return;
    } break;
    case eEvent::famineAllyComply: {
        const auto m = eMessages::favorMessage(
                    inst.fFamineAllyS.fComply);
        showMessage(ed, m, true);
        return;
    } break;
    case eEvent::famineAllyTooLate: {
        const auto m = eMessages::dfavorMessage(
                    inst.fFamineAllyS.fTooLate);
        showMessage(ed, m, true);
        return;
    } break;
    case eEvent::famineAllyRefuse: {
        const auto m = eMessages::dfavorMessage(
                           inst.fFamineAllyS.fRefuse);
        showMessage(ed, m, true);
        return;
    } break;

    case eEvent::famineRivalInitial: {
        showMessage(ed, inst.fFamineRivalD.fInitial, true);
        return;
    } break;
    case eEvent::famineRivalReminder: {
        showMessage(ed, inst.fFamineRivalD.fReminder, true);
        return;
    } break;
    case eEvent::famineRivalOverdue: {
        showMessage(ed, inst.fFamineRivalD.fOverdue, true);
        return;
    } break;
    case eEvent::famineRivalWarning: {
        showMessage(ed, inst.fFamineRivalD.fWarning, true);
        return;
    } break;
    case eEvent::famineRivalComply: {
        const auto m = eMessages::favorMessage(
                    inst.fFamineRivalD.fComply);
        showMessage(ed, m, true);
        return;
    } break;
    case eEvent::famineRivalTooLate: {
        const auto m = eMessages::dfavorMessage(
                    inst.fFamineRivalD.fTooLate);
        showMessage(ed, m, true);
        return;
    } break;
    case eEvent::famineRivalRefuse: {
        const auto m = eMessages::dfavorMessage(
                           inst.fFamineRivalD.fRefuse);
        showMessage(ed, m, true);
        return;
    } break;

    case eEvent::famineSubjectInitial: {
        showMessage(ed, inst.fFamineSubjectP.fInitial, true);
        return;
    } break;
    case eEvent::famineSubjectReminder: {
        showMessage(ed, inst.fFamineSubjectP.fReminder, true);
        return;
    } break;
    case eEvent::famineSubjectOverdue: {
        showMessage(ed, inst.fFamineSubjectP.fOverdue, true);
        return;
    } break;
    case eEvent::famineSubjectWarning: {
        showMessage(ed, inst.fFamineSubjectP.fWarning, true);
        return;
    } break;
    case eEvent::famineSubjectComply: {
        const auto m = eMessages::favorMessage(
                    inst.fFamineSubjectP.fComply);
        showMessage(ed, m, true);
        return;
    } break;
    case eEvent::famineSubjectTooLate: {
        const auto m = eMessages::dfavorMessage(
                    inst.fFamineSubjectP.fTooLate);
        showMessage(ed, m, true);
        return;
    } break;
    case eEvent::famineSubjectRefuse: {
        const auto m = eMessages::dfavorMessage(
                           inst.fFamineSubjectP.fRefuse);
        showMessage(ed, m, true);
        return;
    } break;

    case eEvent::famineParentInitial: {
        showMessage(ed, inst.fFamineParentR.fInitial, true);
        return;
    } break;
    case eEvent::famineParentReminder: {
        showMessage(ed, inst.fFamineParentR.fReminder, true);
        return;
    } break;
    case eEvent::famineParentOverdue: {
        showMessage(ed, inst.fFamineParentR.fOverdue, true);
        return;
    } break;
    case eEvent::famineParentWarning: {
        showMessage(ed, inst.fFamineParentR.fWarning, true);
        return;
    } break;
    case eEvent::famineParentComply: {
        const auto m = eMessages::favorMessage(
                    inst.fFamineParentR.fComply);
        showMessage(ed, m, true);
        return;
    } break;
    case eEvent::famineParentTooLate: {
        const auto m = eMessages::dfavorMessage(
                    inst.fFamineParentR.fTooLate);
        showMessage(ed, m, true);
        return;
    } break;
    case eEvent::famineParentRefuse: {
        const auto m = eMessages::dfavorMessage(
                           inst.fFamineParentR.fRefuse);
        showMessage(ed, m, true);
        return;
    } break;

    case eEvent::projectAllyInitial: {
        showMessage(ed, inst.fProjectAllyS.fInitial, true);
        return;
    } break;
    case eEvent::projectAllyReminder: {
        showMessage(ed, inst.fProjectAllyS.fReminder, true);
        return;
    } break;
    case eEvent::projectAllyOverdue: {
        showMessage(ed, inst.fProjectAllyS.fOverdue, true);
        return;
    } break;
    case eEvent::projectAllyWarning: {
        showMessage(ed, inst.fProjectAllyS.fWarning, true);
        return;
    } break;
    case eEvent::projectAllyComply: {
        const auto m = eMessages::favorMessage(
                    inst.fProjectAllyS.fComply);
        showMessage(ed, m, true);
        return;
    } break;
    case eEvent::projectAllyTooLate: {
        const auto m = eMessages::dfavorMessage(
                    inst.fProjectAllyS.fTooLate);
        showMessage(ed, m, true);
        return;
    } break;
    case eEvent::projectAllyRefuse: {
        const auto m = eMessages::dfavorMessage(
                           inst.fProjectAllyS.fRefuse);
        showMessage(ed, m, true);
        return;
    } break;

    case eEvent::projectRivalInitial: {
        showMessage(ed, inst.fProjectRivalD.fInitial, true);
        return;
    } break;
    case eEvent::projectRivalReminder: {
        showMessage(ed, inst.fProjectRivalD.fReminder, true);
        return;
    } break;
    case eEvent::projectRivalOverdue: {
        showMessage(ed, inst.fProjectRivalD.fOverdue, true);
        return;
    } break;
    case eEvent::projectRivalWarning: {
        showMessage(ed, inst.fProjectRivalD.fWarning, true);
        return;
    } break;
    case eEvent::projectRivalComply: {
        const auto m = eMessages::favorMessage(
                    inst.fProjectRivalD.fComply);
        showMessage(ed, m, true);
        return;
    } break;
    case eEvent::projectRivalTooLate: {
        const auto m = eMessages::dfavorMessage(
                    inst.fProjectRivalD.fTooLate);
        showMessage(ed, m, true);
        return;
    } break;
    case eEvent::projectRivalRefuse: {
        const auto m = eMessages::dfavorMessage(
                           inst.fProjectRivalD.fRefuse);
        showMessage(ed, m, true);
        return;
    } break;

    case eEvent::projectSubjectInitial: {
        showMessage(ed, inst.fProjectSubjectP.fInitial, true);
        return;
    } break;
    case eEvent::projectSubjectReminder: {
        showMessage(ed, inst.fProjectSubjectP.fReminder, true);
        return;
    } break;
    case eEvent::projectSubjectOverdue: {
        showMessage(ed, inst.fProjectSubjectP.fOverdue, true);
        return;
    } break;
    case eEvent::projectSubjectWarning: {
        showMessage(ed, inst.fProjectSubjectP.fWarning, true);
        return;
    } break;
    case eEvent::projectSubjectComply: {
        const auto m = eMessages::favorMessage(
                    inst.fProjectSubjectP.fComply);
        showMessage(ed, m, true);
        return;
    } break;
    case eEvent::projectSubjectTooLate: {
        const auto m = eMessages::dfavorMessage(
                    inst.fProjectSubjectP.fTooLate);
        showMessage(ed, m, true);
        return;
    } break;
    case eEvent::projectSubjectRefuse: {
        const auto m = eMessages::dfavorMessage(
                           inst.fProjectSubjectP.fRefuse);
        showMessage(ed, m, true);
        return;
    } break;

    case eEvent::projectParentInitial: {
        showMessage(ed, inst.fProjectParentR.fInitial, true);
        return;
    } break;
    case eEvent::projectParentReminder: {
        showMessage(ed, inst.fProjectParentR.fReminder, true);
        return;
    } break;
    case eEvent::projectParentOverdue: {
        showMessage(ed, inst.fProjectParentR.fOverdue, true);
        return;
    } break;
    case eEvent::projectParentWarning: {
        showMessage(ed, inst.fProjectParentR.fWarning, true);
        return;
    } break;
    case eEvent::projectParentComply: {
        const auto m = eMessages::favorMessage(
                    inst.fProjectParentR.fComply);
        showMessage(ed, m, true);
        return;
    } break;
    case eEvent::projectParentTooLate: {
        const auto m = eMessages::dfavorMessage(
                    inst.fProjectParentR.fTooLate);
        showMessage(ed, m, true);
        return;
    } break;
    case eEvent::projectParentRefuse: {
        const auto m = eMessages::dfavorMessage(
                           inst.fProjectParentR.fRefuse);
        showMessage(ed, m, true);
        return;
    } break;

    case eEvent::festivalAllyInitial: {
        showMessage(ed, inst.fFestivalAllyS.fInitial, true);
        return;
    } break;
    case eEvent::festivalAllyReminder: {
        showMessage(ed, inst.fFestivalAllyS.fReminder, true);
        return;
    } break;
    case eEvent::festivalAllyOverdue: {
        showMessage(ed, inst.fFestivalAllyS.fOverdue, true);
        return;
    } break;
    case eEvent::festivalAllyWarning: {
        showMessage(ed, inst.fFestivalAllyS.fWarning, true);
        return;
    } break;
    case eEvent::festivalAllyComply: {
        const auto m = eMessages::favorMessage(
                    inst.fFestivalAllyS.fComply);
        showMessage(ed, m, true);
        return;
    } break;
    case eEvent::festivalAllyTooLate: {
        const auto m = eMessages::dfavorMessage(
                    inst.fFestivalAllyS.fTooLate);
        showMessage(ed, m, true);
        return;
    } break;
    case eEvent::festivalAllyRefuse: {
        const auto m = eMessages::dfavorMessage(
                           inst.fFestivalAllyS.fRefuse);
        showMessage(ed, m, true);
        return;
    } break;

    case eEvent::festivalRivalInitial: {
        showMessage(ed, inst.fFestivalRivalD.fInitial, true);
        return;
    } break;
    case eEvent::festivalRivalReminder: {
        showMessage(ed, inst.fFestivalRivalD.fReminder, true);
        return;
    } break;
    case eEvent::festivalRivalOverdue: {
        showMessage(ed, inst.fFestivalRivalD.fOverdue, true);
        return;
    } break;
    case eEvent::festivalRivalWarning: {
        showMessage(ed, inst.fFestivalRivalD.fWarning, true);
        return;
    } break;
    case eEvent::festivalRivalComply: {
        const auto m = eMessages::favorMessage(
                    inst.fFestivalRivalD.fComply);
        showMessage(ed, m, true);
        return;
    } break;
    case eEvent::festivalRivalTooLate: {
        const auto m = eMessages::dfavorMessage(
                    inst.fFestivalRivalD.fTooLate);
        showMessage(ed, m, true);
        return;
    } break;
    case eEvent::festivalRivalRefuse: {
        const auto m = eMessages::dfavorMessage(
                           inst.fFestivalRivalD.fRefuse);
        showMessage(ed, m, true);
        return;
    } break;

    case eEvent::festivalSubjectInitial: {
        showMessage(ed, inst.fFestivalSubjectP.fInitial, true);
        return;
    } break;
    case eEvent::festivalSubjectReminder: {
        showMessage(ed, inst.fFestivalSubjectP.fReminder, true);
        return;
    } break;
    case eEvent::festivalSubjectOverdue: {
        showMessage(ed, inst.fFestivalSubjectP.fOverdue, true);
        return;
    } break;
    case eEvent::festivalSubjectWarning: {
        showMessage(ed, inst.fFestivalSubjectP.fWarning, true);
        return;
    } break;
    case eEvent::festivalSubjectComply: {
        const auto m = eMessages::favorMessage(
                    inst.fFestivalSubjectP.fComply);
        showMessage(ed, m, true);
        return;
    } break;
    case eEvent::festivalSubjectTooLate: {
        const auto m = eMessages::dfavorMessage(
                    inst.fFestivalSubjectP.fTooLate);
        showMessage(ed, m, true);
        return;
    } break;
    case eEvent::festivalSubjectRefuse: {
        const auto m = eMessages::dfavorMessage(
                           inst.fFestivalSubjectP.fRefuse);
        showMessage(ed, m, true);
        return;
    } break;

    case eEvent::festivalParentInitial: {
        showMessage(ed, inst.fFestivalParentR.fInitial, true);
        return;
    } break;
    case eEvent::festivalParentReminder: {
        showMessage(ed, inst.fFestivalParentR.fReminder, true);
        return;
    } break;
    case eEvent::festivalParentOverdue: {
        showMessage(ed, inst.fFestivalParentR.fOverdue, true);
        return;
    } break;
    case eEvent::festivalParentWarning: {
        showMessage(ed, inst.fFestivalParentR.fWarning, true);
        return;
    } break;
    case eEvent::festivalParentComply: {
        const auto m = eMessages::favorMessage(
                    inst.fFestivalParentR.fComply);
        showMessage(ed, m, true);
        return;
    } break;
    case eEvent::festivalParentTooLate: {
        const auto m = eMessages::dfavorMessage(
                    inst.fFestivalParentR.fTooLate);
        showMessage(ed, m, true);
        return;
    } break;
    case eEvent::festivalParentRefuse: {
        const auto m = eMessages::dfavorMessage(
                           inst.fFestivalParentR.fRefuse);
        showMessage(ed, m, true);
        return;
    } break;

    case eEvent::financialWoesAllyInitial: {
        showMessage(ed, inst.fFinancialWoesAllyS.fInitial, true);
        return;
    } break;
    case eEvent::financialWoesAllyReminder: {
        showMessage(ed, inst.fFinancialWoesAllyS.fReminder, true);
        return;
    } break;
    case eEvent::financialWoesAllyOverdue: {
        showMessage(ed, inst.fFinancialWoesAllyS.fOverdue, true);
        return;
    } break;
    case eEvent::financialWoesAllyWarning: {
        showMessage(ed, inst.fFinancialWoesAllyS.fWarning, true);
        return;
    } break;
    case eEvent::financialWoesAllyComply: {
        const auto m = eMessages::favorMessage(
                    inst.fFinancialWoesAllyS.fComply);
        showMessage(ed, m, true);
        return;
    } break;
    case eEvent::financialWoesAllyTooLate: {
        const auto m = eMessages::dfavorMessage(
                    inst.fFinancialWoesAllyS.fTooLate);
        showMessage(ed, m, true);
        return;
    } break;
    case eEvent::financialWoesAllyRefuse: {
        const auto m = eMessages::dfavorMessage(
                           inst.fFinancialWoesAllyS.fRefuse);
        showMessage(ed, m, true);
        return;
    } break;

    case eEvent::financialWoesRivalInitial: {
        showMessage(ed, inst.fFinancialWoesRivalD.fInitial, true);
        return;
    } break;
    case eEvent::financialWoesRivalReminder: {
        showMessage(ed, inst.fFinancialWoesRivalD.fReminder, true);
        return;
    } break;
    case eEvent::financialWoesRivalOverdue: {
        showMessage(ed, inst.fFinancialWoesRivalD.fOverdue, true);
        return;
    } break;
    case eEvent::financialWoesRivalWarning: {
        showMessage(ed, inst.fFinancialWoesRivalD.fWarning, true);
        return;
    } break;
    case eEvent::financialWoesRivalComply: {
        const auto m = eMessages::favorMessage(
                    inst.fFinancialWoesRivalD.fComply);
        showMessage(ed, m, true);
        return;
    } break;
    case eEvent::financialWoesRivalTooLate: {
        const auto m = eMessages::dfavorMessage(
                    inst.fFinancialWoesRivalD.fTooLate);
        showMessage(ed, m, true);
        return;
    } break;
    case eEvent::financialWoesRivalRefuse: {
        const auto m = eMessages::dfavorMessage(
                           inst.fFinancialWoesRivalD.fRefuse);
        showMessage(ed, m, true);
        return;
    } break;

    case eEvent::financialWoesSubjectInitial: {
        showMessage(ed, inst.fFinancialWoesSubjectP.fInitial, true);
        return;
    } break;
    case eEvent::financialWoesSubjectReminder: {
        showMessage(ed, inst.fFinancialWoesSubjectP.fReminder, true);
        return;
    } break;
    case eEvent::financialWoesSubjectOverdue: {
        showMessage(ed, inst.fFinancialWoesSubjectP.fOverdue, true);
        return;
    } break;
    case eEvent::financialWoesSubjectWarning: {
        showMessage(ed, inst.fFinancialWoesSubjectP.fWarning, true);
        return;
    } break;
    case eEvent::financialWoesSubjectComply: {
        const auto m = eMessages::favorMessage(
                    inst.fFinancialWoesSubjectP.fComply);
        showMessage(ed, m, true);
        return;
    } break;
    case eEvent::financialWoesSubjectTooLate: {
        const auto m = eMessages::dfavorMessage(
                    inst.fFinancialWoesSubjectP.fTooLate);
        showMessage(ed, m, true);
        return;
    } break;
    case eEvent::financialWoesSubjectRefuse: {
        const auto m = eMessages::dfavorMessage(
                           inst.fFinancialWoesSubjectP.fRefuse);
        showMessage(ed, m, true);
        return;
    } break;

    case eEvent::financialWoesParentInitial: {
        showMessage(ed, inst.fFinancialWoesParentR.fInitial, true);
        return;
    } break;
    case eEvent::financialWoesParentReminder: {
        showMessage(ed, inst.fFinancialWoesParentR.fReminder, true);
        return;
    } break;
    case eEvent::financialWoesParentOverdue: {
        showMessage(ed, inst.fFinancialWoesParentR.fOverdue, true);
        return;
    } break;
    case eEvent::financialWoesParentWarning: {
        showMessage(ed, inst.fFinancialWoesParentR.fWarning, true);
        return;
    } break;
    case eEvent::financialWoesParentComply: {
        const auto m = eMessages::favorMessage(
                    inst.fFinancialWoesParentR.fComply);
        showMessage(ed, m, true);
        return;
    } break;
    case eEvent::financialWoesParentTooLate: {
        const auto m = eMessages::dfavorMessage(
                    inst.fFinancialWoesParentR.fTooLate);
        showMessage(ed, m, true);
        return;
    } break;
    case eEvent::financialWoesParentRefuse: {
        const auto m = eMessages::dfavorMessage(
                           inst.fFinancialWoesParentR.fRefuse);
        showMessage(ed, m, true);
        return;
    } break;

    case eEvent::unemployment: {
        showMessage(ed, inst.fUnemployment, true);
        return;
    } break;
    case eEvent::employees: {
        showMessage(ed, inst.fEmployees, true);
        return;
    } break;
    case eEvent::plague: {
        showMessage(ed, inst.fPlague, true);
    } break;

    case eEvent::cityConquered: {
        showMessage(ed, inst.fCityConqueredByPlayer, true);
        return;
    } break;
    case eEvent::cityConquerFailed: {
        showMessage(ed, inst.fCityConquerFailedByPlayer, true);
        return;
    } break;


    case eEvent::raidGranted: {
        showMessage(ed, inst.fRaidGranted.fGranted);
        return;
    } break;
    case eEvent::raidPartialSpace: {
        showMessage(ed, inst.fRaidGranted.fPartialSpace);
        return;
    } break;
    case eEvent::raidInsufficientSpace: {
        showMessage(ed, inst.fRaidGranted.fInsufficientSpace);
        return;
    } break;
    case eEvent::raidLastChance: {
        showMessage(ed, inst.fRaidGranted.fLastChance);
        return;
    } break;
    case eEvent::raidAccepted: {
        showMessage(ed, inst.fRaidGranted.fAccepted, true);
        return;
    } break;
    case eEvent::raidPostponed: {
        showMessage(ed, inst.fRaidGranted.fPostponed, true);
        return;
    } break;
    case eEvent::raidRefused: {
        showMessage(ed, inst.fRaidGranted.fRefused, true);
        return;
    } break;
    case eEvent::raidForfeited: {
        showMessage(ed, inst.fRaidGranted.fForfeited, true);
        return;
    } break;
    case eEvent::raidCashGranted: {
        showMessage(ed, inst.fRaidGranted.fCashGranted, true);
        return;
    } break;
    case eEvent::raidCashAccepted: {
        showMessage(ed, inst.fRaidGranted.fCashAccepted, true);
        return;
    } break;

    case eEvent::cityRaidFailed: {
        showMessage(ed, inst.fCityRaidFailedByPlayer, true);
        return;
    } break;

    case eEvent::allyAttackedByPlayer: {
        showMessage(ed, inst.fAllyAttackedByPlayer, true);
        return;
    } break;

    case eEvent::armyReturns: {
        showMessage(ed, inst.fArmyReturns, true);
    } break;

    case eEvent::militaryBuildup: {
        showMessage(ed, inst.fMilitaryBuildup, true);
        return;
    } break;
    case eEvent::militaryDecline: {
        showMessage(ed, inst.fMilitaryDecline, true);
        return;
    } break;
    case eEvent::tradeShutdowns: {
        showMessage(ed, inst.fTradeShutsDown, true);
        return;
    } break;
    case eEvent::tradeOpensUp: {
        showMessage(ed, inst.fTradeOpensUp, true);
        return;
    } break;
    case eEvent::supplyIncrease: {
        showMessage(ed, inst.fSupplyIncrease, true);
        return;
    } break;
    case eEvent::supplyDecrease: {
        showMessage(ed, inst.fSupplyDecrease, true);
        return;
    } break;
    case eEvent::demandIncrease: {
        showMessage(ed, inst.fDemandIncrease, true);
        return;
    } break;
    case eEvent::demandDecrease: {
        showMessage(ed, inst.fDemandDecrease, true);
        return;
    } break;
    case eEvent::priceIncrease: {
        showMessage(ed, inst.fPriceIncrease, true);
        return;
    } break;
    case eEvent::priceDecrease: {
        showMessage(ed, inst.fPriceDecrease, true);
        return;
    } break;
    case eEvent::wageIncrease: {
        showMessage(ed, inst.fWageIncrease, true);
        return;
    } break;
    case eEvent::wageDecrease: {
        showMessage(ed, inst.fWageDecrease, true);
        return;
    } break;
    case eEvent::economicProsperity: {
        showMessage(ed, inst.fEconomicProsperity, true);
        return;
    } break;
    case eEvent::economicDecline: {
        showMessage(ed, inst.fEconomicDecline, true);
        return;
    } break;

    case eEvent::troopsRequestVassalInitial: {
        showMessage(ed, inst.fVassalTroopsRequest.fInitialAnnouncement, true);
        return;
    } break;
    case eEvent::troopsRequestColonyInitial: {
        showMessage(ed, inst.fColonyTroopsRequest.fInitialAnnouncement, true);
        return;
    } break;
    case eEvent::troopsRequestParentCityInitial: {
        showMessage(ed, inst.fParentCityTroopsRequest.fInitialAnnouncement, true);
        return;
    } break;
    case eEvent::troopsRequestAllyInitial: {
        showMessage(ed, inst.fAllyTroopsRequest.fInitialAnnouncement, true);
        return;
    } break;

    case eEvent::troopsRequestVassalFirstReminder: {
        showMessage(ed, inst.fVassalTroopsRequest.fFirstReminder, true);
        return;
    } break;
    case eEvent::troopsRequestColonyFirstReminder: {
        showMessage(ed, inst.fColonyTroopsRequest.fFirstReminder, true);
        return;
    } break;
    case eEvent::troopsRequestParentCityFirstReminder: {
        showMessage(ed, inst.fParentCityTroopsRequest.fFirstReminder, true);
        return;
    } break;
    case eEvent::troopsRequestAllyFirstReminder: {
        showMessage(ed, inst.fAllyTroopsRequest.fFirstReminder, true);
        return;
    } break;

    case eEvent::troopsRequestVassalLastReminder: {
        showMessage(ed, inst.fVassalTroopsRequest.fLastReminder, true);
        return;
    } break;
    case eEvent::troopsRequestColonyLastReminder: {
        showMessage(ed, inst.fColonyTroopsRequest.fLastReminder, true);
        return;
    } break;
    case eEvent::troopsRequestParentCityLastReminder: {
        showMessage(ed, inst.fParentCityTroopsRequest.fLastReminder, true);
        return;
    } break;
    case eEvent::troopsRequestAllyLastReminder: {
        showMessage(ed, inst.fAllyTroopsRequest.fLastReminder, true);
        return;
    } break;

    case eEvent::troopsRequestVassalConquered: {
        showMessage(ed, inst.fVassalTroopsRequest.fConquered, true);
        return;
    } break;
    case eEvent::troopsRequestColonyConquered: {
        showMessage(ed, inst.fColonyTroopsRequest.fConquered, true);
        return;
    } break;
    case eEvent::troopsRequestParentCityConquered: {
        showMessage(ed, inst.fParentCityTroopsRequest.fConquered, true);
        return;
    } break;
    case eEvent::troopsRequestAllyConquered: {
        showMessage(ed, inst.fAllyTroopsRequest.fConquered, true);
        return;
    } break;

    case eEvent::troopsRequestAttackAverted: {
        showMessage(ed, inst.fTroopsRequestAttackAverted, true);
        return;
    } break;

    case eEvent::godDisaster: {
        const auto msgs = inst.godMessages(ed.fGod);
        showMessage(ed, msgs->fDisaster);
        return;
    } break;
    case eEvent::godDisasterEnds: {
        const auto msgs = inst.godMessages(ed.fGod);
        showMessage(ed, msgs->fDisasterEnds);
        return;
    } break;
    case eEvent::godTradeResumes: {
        if(ed.fGod == eGodType::zeus) {
            showMessage(ed, inst.fZeusTradeResumes);
        } else if(ed.fGod == eGodType::poseidon) {
            showMessage(ed, inst.fPoseidonTradeResumes);
        } else if(ed.fGod == eGodType::hermes) {
            showMessage(ed, inst.fHermesTradeResumes);
        }
        return;
    } break;
    case eEvent::aidArrives: {
        showMessage(ed, inst.fSupportTroopsArrive);
    } break;
    case eEvent::aidDeparts: {
        showMessage(ed, inst.fSupportTroopsDepart);
        return;
    } break;
    case eEvent::aidDefeated: {
        showMessage(ed, inst.fSupportTroopsDefeated);
        return;
    } break;
    case eEvent::strikeDeparture: {
        showMessage(ed, inst.fStrikeDepart);
        return;
    } break;
    case eEvent::strikeSuccessful: {
        showMessage(ed, inst.fStrikeSuccessful);
        return;
    } break;
    case eEvent::strikeUnsuccessful: {
        showMessage(ed, inst.fStrikeUnsuccessul);
        return;
    } break;
    case eEvent::rivalArmyAway: {
        showMessage(ed, inst.fRivalArmyAway);
        return;
    } break;

    case eEvent::earthquake: {
        showMessage(ed, inst.fEarthqauke);
    } break;

    case eEvent::cityBecomesAlly: {
        showMessage(ed, inst.fCityBecomesAlly);
        return;
    } break;
    case eEvent::cityBecomesRival: {
        showMessage(ed, inst.fCityBecomesRival);
        return;
    } break;
    case eEvent::cityBecomesVassal: {
        showMessage(ed, inst.fCityBecomesVassal);
        return;
    } break;

    case eEvent::cityBecomesActive: {
        showMessage(ed, inst.fCityBecomesActive);
        return;
    } break;
    case eEvent::cityBecomesInactive: {
        showMessage(ed, inst.fCityBecomesInactive);
        return;
    } break;

    case eEvent::cityBecomesVisible: {
        showMessage(ed, inst.fCityBecomesVisible);
        return;
    } break;
    case eEvent::cityBecomesInvisible: {
        showMessage(ed, inst.fCityBecomesInvisible);
        return;
    } break;

    case eEvent::rivalConqueredByAlly: {
        showMessage(ed, inst.fRivalConqueredByAlly);
        return;
    } break;
    case eEvent::rivalConqueredByVassal: {
        showMessage(ed, inst.fRivalConqueredByVassal);
        return;
    } break;

    case eEvent::allyConqueredByRival: {
        showMessage(ed, inst.fAllyConqueredByRival);
        return;
    } break;
    case eEvent::vassalConqueredByRival: {
        showMessage(ed, inst.fVassalConqueredByRival);
        return;
    } break;
    case eEvent::colonyConqueredByRival: {
        showMessage(ed, inst.fColonyConqueredByRival);
        return;
    } break;
    case eEvent::parentConqueredByRival: {
        showMessage(ed, inst.fParentConqueredByRival);
        return;
    } break;

    case eEvent::colonyRestored: {
        showMessage(ed, inst.fColonyRestored);
        return;
    } break;

    case eEvent::debtAnniversary: {
        showMessage(ed, inst.fDebtAnniversary);
        return;
    } break;

    case eEvent::colonyMonument: {
        showMessage(ed, inst.fColonyMonument);
        return;
    } break;
    }
    mGm->pushEvent(e, ed);
}

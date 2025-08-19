#include "eeventwidgetbase.h"

#include "gameEvents/egameevent.h"

#include "widgets/evaluebutton.h"
#include "engine/egameboard.h"
#include "widgets/elabeledwidget.h"
#include "elanguage.h"
#include "emainwindow.h"
#include "widgets/etriggerselectionwidget.h"
#include "widgets/eeventselectionwidget.h"

#include "widgets/eresourcebutton.h"
#include "widgets/ecitybutton.h"
#include "widgets/egodbutton.h"
#include "widgets/eswitchbutton.h"

#include "gameEvents/egoddisasterevent.h"
#include "gameEvents/erivalarmyawayevent.h"
#include "gameEvents/ecitybecomesevent.h"
#include "gameEvents/ecityevent.h"
#include "gameEvents/egodeventvalue.h"
#include "gameEvents/egodreasoneventvalue.h"

#include "egodattackeventwidget.h"
#include "emonsterattackeventwidget.h"
#include "einvasioneventwidget.h"
#include "ereceiverequesteventwidget.h"
#include "egodquesteventwidget.h"
#include "etroopsrequesteventwidget.h"
#include "egoddisastereventwidget.h"
#include "erivalarmyawayeventwidget.h"
#include "ecitybecomeseventwidget.h"

void eEventWidgetBase::initialize(const stdsptr<eGameEvent>& e) {
    setType(eFrameType::message);

    const auto cont = new eWidget(window());
    cont->setNoPadding();
    addWidget(cont);
    const int p = padding();
    cont->move(2*p, 2*p);
    cont->resize(width() - 4*p, height() - 4*p);

    const auto leftW = new eWidget(window());
    leftW->setNoPadding();

    if(const auto ee = dynamic_cast<ePointEventBase*>(e.get())) {
        const auto countL = new eLabeledWidget(window());
        const auto countW = new eWidget(window());
        const int p = countW->padding();
        countW->setNoPadding();
        {
            const auto minCountB = new eValueButton(window());
            minCountB->setValueChangeAction([ee](const int p) {
                ee->setMinPointId(p);
            });
            minCountB->initialize(1, 999);
            const int rc = ee->minPointId();
            minCountB->setValue(rc);
            countW->addWidget(minCountB);
        }
        {
            const auto maxCountB = new eValueButton(window());
            maxCountB->setValueChangeAction([ee](const int p) {
                ee->setMaxPointId(p);
            });
            maxCountB->initialize(1, 999);
            const int rc = ee->maxPointId();
            maxCountB->setValue(rc);
            countW->addWidget(maxCountB);
        }

        countW->stackHorizontally(p);
        countW->fitContent();
        countL->setup(eLanguage::zeusText(44, 362), countW);
        leftW->addWidget(countL);
    }

    if(const auto ee = dynamic_cast<eCountEvent*>(e.get())) {
        const auto countL = new eLabeledWidget(window());
        const auto countW = new eWidget(window());
        countW->setNoPadding();
        {
            const auto minCountB = new eValueButton(window());
            minCountB->setValueChangeAction([ee](const int p) {
                ee->setMinCount(p);
            });
            minCountB->initialize(1, 999);
            const int rc = ee->minCount();
            minCountB->setValue(rc);
            countW->addWidget(minCountB);
        }
        {
            const auto maxCountB = new eValueButton(window());
            maxCountB->setValueChangeAction([ee](const int p) {
                ee->setMaxCount(p);
            });
            maxCountB->initialize(1, 999);
            const int rc = ee->maxCount();
            maxCountB->setValue(rc);
            countW->addWidget(maxCountB);
        }

        countW->stackHorizontally(p);
        countW->fitContent();
        countL->setup(eLanguage::zeusText(44, 361), countW);
        leftW->addWidget(countL);
    }

    if(const auto ee = dynamic_cast<eResourceEvent*>(e.get())) {
        for(int i = 0; i < 3; i++) {
            const auto resourceTypeButton = new eResourceButton(window());
            resourceTypeButton->initialize([i, ee](const eResourceType r){
                ee->setResourceType(i, r);
            }, eResourceType::allBasic, true, true);
            const auto rr = ee->resourceType(i);
            resourceTypeButton->setResource(rr);
            leftW->addWidget(resourceTypeButton);
        }
    }

    if(const auto ee = dynamic_cast<eCityEvent*>(e.get())) {
        const auto cityButtonL = new eLabeledWidget(window());
        const auto cityButton = new eCityButton(window());
        const auto board = e->worldBoard();
        cityButton->initialize(board, [ee](const stdsptr<eWorldCity>& c){
            ee->setCity(c);
        });
        const auto cc = ee->city();
        cityButton->setCity(cc);
        cityButtonL->setup(eLanguage::zeusText(44, 359), cityButton);
        leftW->addWidget(cityButtonL);
    }

    if(const auto ee = dynamic_cast<eGodReasonEventValue*>(e.get())) {
        const auto godButtonL = new eLabeledWidget(window());
        const auto godButton = new eSwitchButton(window());
        godButton->setSwitchAction([ee](const int v) {
            ee->setGodReason(v);
        });
        godButton->addValue(eLanguage::zeusText(18, 0));
        godButton->addValue(eLanguage::zeusText(18, 1));
        godButton->fitValidContent();
        const auto iniT = ee->godReason() ? 1 : 0;
        godButton->setValue(iniT);
        godButtonL->setup(eLanguage::zeusText(44, 215), godButton);
        leftW->addWidget(godButtonL);
    }

    if(const auto ee = dynamic_cast<eGodEventValue*>(e.get())) {
        const auto godButtonL = new eLabeledWidget(window());
        const auto act = [ee](const eGodType type) {
            ee->setGod(type);
        };
        const auto godButton = new eGodButton(window());
        godButton->initialize(act);
        const auto iniT = ee->god();
        godButton->setType(iniT);
        godButtonL->setup(eLanguage::zeusText(44, 215), godButton);
        leftW->addWidget(godButtonL);
    }

    const auto et = e->type();
    switch(et) {
    case eGameEventType::godAttack: {
        const auto eew = new eGodAttackEventWidget(window());
        const auto gaee = static_cast<eGodAttackEvent*>(e.get());
        eew->initialize(this, gaee);
        leftW->addWidget(eew);
    } break;
    case eGameEventType::monsterUnleashed:
    case eGameEventType::monsterInvasion:
    case eGameEventType::monsterInCity: {
        const auto eew = new eMonsterAttackEventWidget(window());
        const auto maee = static_cast<eMonsterInvasionEventBase*>(e.get());
        eew->initialize(maee);
        leftW->addWidget(eew);
    } break;
    case eGameEventType::invasion: {
        const auto eew = new eInvasionEventWidget(window());
        const auto iee = static_cast<eInvasionEvent*>(e.get());
        eew->initialize(iee);
        leftW->addWidget(eew);
    } break;
    case eGameEventType::receiveRequest: {
        const auto eew = new eReceiveRequestEventWidget(window());
        const auto rree = static_cast<eReceiveRequestEvent*>(e.get());
        eew->initialize(rree);
        leftW->addWidget(eew);
    } break;
    case eGameEventType::troopsRequest: {
        const auto eew = new eTroopsRequestEventWidget(window());
        const auto rree = static_cast<eTroopsRequestEvent*>(e.get());
        eew->initialize(rree);
        leftW->addWidget(eew);
    } break;
    case eGameEventType::godQuest:
    case eGameEventType::godQuestFulfilled: {
        const auto eew = new eGodQuestEventWidget(window());
        const auto gqee = static_cast<eGodQuestEventBase*>(e.get());
        eew->initialize(gqee);
        leftW->addWidget(eew);
    } break;
    case eGameEventType::godDisaster: {
        const auto eew = new eGodDisasterEventWidget(window());
        const auto emceb = static_cast<eGodDisasterEvent*>(e.get());
        eew->initialize(emceb);
        leftW->addWidget(eew);
    } break;
    case eGameEventType::rivalArmyAway: {
        const auto eew = new eRivalArmyAwayEventWidget(window());
        const auto emceb = static_cast<eRivalArmyAwayEvent*>(e.get());
        eew->initialize(emceb);
        leftW->addWidget(eew);
    } break;
    case eGameEventType::cityBecomes: {
        const auto eew = new eCityBecomesEventWidget(window());
        const auto ee = static_cast<eCityBecomesEvent*>(e.get());
        eew->initialize(ee);
        leftW->addWidget(eew);
    } break;
    default:
        break;
    }

    const auto dateW = new eWidget(window());
    dateW->setNoPadding();

    const auto yearsButtonL = new eLabeledWidget(window());
    const auto yearsButton = new eValueButton(window());
    yearsButton->setValueChangeAction([e, yearsButton](const int y) {
        e->setDatePlusYears(y);
        yearsButton->setText("+" + yearsButton->text());
    });
    yearsButton->initialize(0, 99999);
    yearsButton->setValue(e->datePlusYears());
    yearsButton->setText("+" + yearsButton->text());
    yearsButtonL->setup(eLanguage::text("years:"), yearsButton);
    dateW->addWidget(yearsButtonL);

    const auto monthssButtonL = new eLabeledWidget(window());
    const auto monthsButton = new eValueButton(window());
    monthsButton->setValueChangeAction([e, monthsButton](const int y) {
        e->setDatePlusMonths(y);
        monthsButton->setText("+" + monthsButton->text());
    });
    monthsButton->initialize(0, 99999);
    monthsButton->setValue(e->datePlusMonths());
    monthsButton->setText("+" + monthsButton->text());
    monthssButtonL->setup(eLanguage::text("months:"), monthsButton);
    dateW->addWidget(monthssButtonL);

    const auto daysButtonL = new eLabeledWidget(window());
    const auto daysButton = new eValueButton(window());
    daysButton->setValueChangeAction([e, daysButton](const int y) {
        e->setDatePlusDays(y);
        daysButton->setText("+" + daysButton->text());
    });
    daysButton->initialize(0, 99999);
    daysButton->setValue(e->datePlusDays());
    daysButton->setText("+" + daysButton->text());
    daysButtonL->setup(eLanguage::text("days:"), daysButton);
    dateW->addWidget(daysButtonL);

    const auto periodButtonL = new eLabeledWidget(window());
    const auto periodButton = new eValueButton(window());
    periodButton->setValueChangeAction([e](const int p) {
        e->setPeriod(p);
    });
    periodButton->initialize(31, 99999);
    periodButton->setValue(e->period());
    periodButtonL->setup(eLanguage::text("period:"), periodButton);
    dateW->addWidget(periodButtonL);

    const auto repeatButtonL = new eLabeledWidget(window());
    const auto repeatButton = new eValueButton(window());
    repeatButton->setValueChangeAction([e](const int p) {
        e->setRepeat(p);
    });
    repeatButton->initialize(0, 99999);
    repeatButton->setValue(e->repeat());
    repeatButtonL->setup(eLanguage::text("repeat:"), repeatButton);
    dateW->addWidget(repeatButtonL);

    const auto& ts = e->triggers();
    if(!ts.empty()) {
        const int ec = e->triggerEventsCount();
        const auto ecStr = std::to_string(ec);
        const auto str = eLanguage::text("triggers") + " (" + ecStr + ")";
        const auto triggersButt = new eFramedButton(str, window());
        triggersButt->setUnderline(false);
        triggersButt->fitContent();
        triggersButt->setPressAction([this, e]() {
            const auto choose = new eTriggerSelectionWidget(window());

            choose->resize(width(), height());
            std::vector<std::string> labels;
            const auto& ts = e->triggers();
            for(const auto& t : ts) {
                const int ec = t->eventCount();
                const auto ecStr = std::to_string(ec);
                labels.push_back("(" + ecStr + ") " + t->name());
            }
            const auto act = [this, e](const int id) {
                const auto& ts = e->triggers();
                const auto& t = ts[id];

                const auto choose = new eEventSelectionWidget(
                                        eGameEventBranch::trigger,
                                        window());

                choose->resize(width(), height());
                const auto get = [t]() {
                    return t->events();
                };
                const auto add = [t](const stdsptr<eGameEvent>& e) {
                    t->addEvent(e);
                };
                const auto remove = [t](const stdsptr<eGameEvent>& e) {
                    t->removeEvent(e);
                };
                auto& board = *e->gameBoard();
                choose->initialize(e->cityId(), get, add, remove, board);

                window()->execDialog(choose);
                choose->align(eAlignment::center);
            };
            choose->initialize(labels, act);

            window()->execDialog(choose);
            choose->align(eAlignment::center);
        });
        dateW->addWidget(triggersButt);
    }

    leftW->stackVertically(p);
    leftW->fitContent();
    cont->addWidget(leftW);
    dateW->stackVertically(p);
    dateW->fitContent();
    cont->addWidget(dateW);
    cont->layoutHorizontallyWithoutSpaces();
}

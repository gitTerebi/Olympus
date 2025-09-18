#include "eeventwidgetbase.h"

#include "gameEvents/egameevent.h"

#include "widgets/evaluebutton.h"
#include "widgets/etypebutton.h"
#include "engine/egameboard.h"
#include "widgets/elabeledwidget.h"
#include "elanguage.h"
#include "emainwindow.h"
#include "widgets/etriggerselectionwidget.h"
#include "widgets/eeventselectionwidget.h"

#include "widgets/eresourcebutton.h"
#include "widgets/ecitybutton.h"
#include "widgets/egodbutton.h"
#include "widgets/emonsterbutton.h"
#include "widgets/eswitchbutton.h"

#include "gameEvents/egoddisasterevent.h"
#include "gameEvents/ecitybecomesevent.h"
#include "gameEvents/ecityeventvalue.h"
#include "gameEvents/egodeventvalue.h"
#include "gameEvents/egodreasoneventvalue.h"
#include "gameEvents/eattackingcityeventvalue.h"
#include "gameEvents/ereceiverequestevent.h"
#include "gameEvents/etroopsrequestevent.h"
#include "gameEvents/emonsterseventvalue.h"
#include "gameEvents/emonsterinvasioneventbase.h"
#include "gameEvents/etidalwaveevent.h"

#include "egodattackeventwidget.h"
#include "einvasioneventwidget.h"
#include "egodquesteventwidget.h"
#include "egoddisastereventwidget.h"

#include "evectorhelpers.h"

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

    if(const auto ee = dynamic_cast<eCityBecomesEvent*>(e.get())) {
        const auto typeButtonL = new eLabeledWidget(window());
        const std::vector<eCityBecomesType> types{eCityBecomesType::ally,
                                                  eCityBecomesType::rival,
                                                  eCityBecomesType::vassal,

                                                  eCityBecomesType::active,
                                                  eCityBecomesType::inactive,

                                                  eCityBecomesType::visible,
                                                  eCityBecomesType::invisible,

                                                  eCityBecomesType::rebellionOver,
                                                  eCityBecomesType::conquered};
        const std::vector<std::string> typeNames{eLanguage::zeusText(253, 0),
                                                 eLanguage::zeusText(253, 1),
                                                 eLanguage::zeusText(253, 2),

                                                 eLanguage::zeusText(44, 248),
                                                 eLanguage::zeusText(44, 249),

                                                 eLanguage::zeusText(44, 307),
                                                 eLanguage::zeusText(44, 306),

                                                 eLanguage::zeusText(35, 23),
                                                 eLanguage::zeusText(35, 24)};

        const auto typeButton = new eTypeButton(window());
        const auto type = ee->type();
        const int itype = static_cast<int>(type);
        typeButton->initialize(itype, typeNames, [ee, types](const int val) {
            const auto t = types[val];
            ee->setType(t);
        });

        typeButtonL->setup(eLanguage::zeusText(44, 358), typeButton);
        leftW->addWidget(typeButtonL);
    }

    if(const auto ee = dynamic_cast<eReceiveRequestEvent*>(e.get())) {
        const auto typeButtonL = new eLabeledWidget(window());
        const std::vector<eReceiveRequestType> types {
            eReceiveRequestType::general,
            eReceiveRequestType::festival,
            eReceiveRequestType::project,
            eReceiveRequestType::famine,
            eReceiveRequestType::financialWoes
        };
        const std::vector<std::string> typeNames {
            eLanguage::zeusText(290, 1),
            eLanguage::zeusText(290, 2),
            eLanguage::zeusText(290, 3),
            eLanguage::zeusText(290, 4),
            eLanguage::zeusText(290, 5)
        };
        const auto typeButton = new eTypeButton(window());
        const auto type = ee->requestType();
        const int itype = eVectorHelpers::index(types, type);
        typeButton->initialize(itype, typeNames, [ee, types](const int val) {
            const auto type = types[val];
            ee->setRequestType(type);
        });

        typeButtonL->setup(eLanguage::zeusText(44, 358), typeButton);
        leftW->addWidget(typeButtonL);
    }

    if(const auto ee = dynamic_cast<eTroopsRequestEvent*>(e.get())) {
        const auto typeButtonL = new eLabeledWidget(window());
        const std::vector<eTroopsRequestEventType> types {
            eTroopsRequestEventType::cityUnderAttack,
            eTroopsRequestEventType::cityAttacksRival,
            eTroopsRequestEventType::greekCityTerrorized
        };
        const std::vector<std::string> typeNames {
            eLanguage::zeusText(290, 7),
            eLanguage::zeusText(290, 8),
            eLanguage::zeusText(290, 9)
        };
        const auto typeButton = new eTypeButton(window());
        const auto type = ee->type();
        const int itype = eVectorHelpers::index(types, type);
        typeButton->initialize(itype, typeNames, [ee, types](const int val) {
            const auto type = types[val];
            ee->setType(type);
        });

        typeButtonL->setup(eLanguage::zeusText(44, 358), typeButton);
        leftW->addWidget(typeButtonL);

        const auto effectButtonL = new eLabeledWidget(window());
        const auto effectButton = new eSwitchButton(window());
        effectButton->setUnderline(false);
        effectButton->addValue(eLanguage::zeusText(44, 287));
        effectButton->addValue(eLanguage::zeusText(44, 288));
        effectButton->addValue(eLanguage::zeusText(44, 289));
        effectButton->fitValidContent();
        const auto eff = ee->effect();
        const int ieff = static_cast<int>(eff);
        effectButton->setValue(ieff);

        effectButton->setSwitchAction([ee](const int val) {
            const auto effect = static_cast<eTroopsRequestEventEffect>(val);
            ee->setEffect(effect);
        });

        effectButtonL->setup(eLanguage::zeusText(44, 286), effectButton);
        leftW->addWidget(effectButtonL);
    }

    if(const auto ee = dynamic_cast<ePointEventValue*>(e.get())) {
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

    if(const auto ee = dynamic_cast<eCountEventValue*>(e.get())) {
        const auto countL = new eLabeledWidget(window());
        const auto countW = new eWidget(window());
        countW->setNoPadding();
        {
            const auto minCountB = new eValueButton(window());
            minCountB->setValueChangeAction([ee](const int p) {
                ee->setMinCount(p);
            });
            minCountB->initialize(-99999, 99999);
            const int rc = ee->minCount();
            minCountB->setValue(rc);
            countW->addWidget(minCountB);
        }
        {
            const auto maxCountB = new eValueButton(window());
            maxCountB->setValueChangeAction([ee](const int p) {
                ee->setMaxCount(p);
            });
            maxCountB->initialize(-99999, 99999);
            const int rc = ee->maxCount();
            maxCountB->setValue(rc);
            countW->addWidget(maxCountB);
        }

        countW->stackHorizontally(p);
        countW->fitContent();
        countL->setup(eLanguage::zeusText(44, 361), countW);
        leftW->addWidget(countL);
    }

    if(const auto ee = dynamic_cast<eResourceEventValue*>(e.get())) {
        const auto buttonsL = new eLabeledWidget(window());
        const auto widget = new eWidget(window());
        widget->setNoPadding();
        for(int i = 0; i < 3; i++) {
            const auto resourceTypeButton = new eResourceButton(window());
            resourceTypeButton->initialize([i, ee](const eResourceType r){
                ee->setResourceType(i, r);
            }, eResourceType::allBasic | eResourceType::drachmas, true, true);
            const auto rr = ee->resourceType(i);
            resourceTypeButton->setResource(rr);
            widget->addWidget(resourceTypeButton);
        }
        widget->stackVertically(p);
        widget->fitContent();
        buttonsL->setup(eLanguage::zeusText(44, 360), widget);
        leftW->addWidget(buttonsL);
    }

    if(const auto ee = dynamic_cast<eMonstersEventValue*>(e.get())) {
        const auto buttonsL = new eLabeledWidget(window());
        const auto widget = new eWidget(window());
        widget->setNoPadding();
        const auto et = e->eGameEvent::type();
        const bool withGodsOnly = et == eGameEventType::monsterUnleashed;
        for(int i = 0; i < 3; i++) {
            const auto monsterTypeButton = new eMonsterButton(window());
            monsterTypeButton->initialize([i, ee](const eMonsterType m){
                ee->setMonsterType(i, m);
            }, withGodsOnly);
            bool valid;
            const auto mm = ee->monsterType(i, valid);
            if(valid) monsterTypeButton->setType(mm);
            else monsterTypeButton->setText("");
            widget->addWidget(monsterTypeButton);
        }
        widget->stackVertically(p);
        widget->fitContent();
        buttonsL->setup(eLanguage::zeusText(44, 360), widget);
        leftW->addWidget(buttonsL);
    }

    if(const auto ee = dynamic_cast<eMonsterInvasionEventBase*>(e.get())) {
        const auto buttonL = new eLabeledWidget(window());
        const auto aggressivnessButton = new eSwitchButton(window());
        aggressivnessButton->setUnderline(false);
        aggressivnessButton->addValue(eLanguage::zeusText(94, 0));
        aggressivnessButton->addValue(eLanguage::zeusText(94, 1));
        aggressivnessButton->addValue(eLanguage::zeusText(94, 2));
        aggressivnessButton->addValue(eLanguage::zeusText(94, 3));
        aggressivnessButton->fitValidContent();
        const auto a = ee->aggressivness();
        const int v = static_cast<int>(a);
        aggressivnessButton->setValue(v);
        aggressivnessButton->setSwitchAction([ee](const int v) {
            const auto a = static_cast<eMonsterAggressivness>(v);
            ee->setAggressivness(a);
        });
        buttonL->setup(eLanguage::zeusText(44, 177), aggressivnessButton);
        leftW->addWidget(buttonL);
    }

    if(const auto ee = dynamic_cast<eTidalWaveEvent*>(e.get())) {
        const auto buttonL = new eLabeledWidget(window());
        const auto permanentButton = new eSwitchButton(window());
        permanentButton->setUnderline(false);
        permanentButton->addValue(eLanguage::zeusText(18, 0));
        permanentButton->addValue(eLanguage::zeusText(18, 1));
        permanentButton->fitValidContent();
        const bool p = ee->permanent();
        permanentButton->setValue(p ? 1 : 0);
        permanentButton->setSwitchAction([ee](const int v) {
            ee->setPermanent(v);
        });
        buttonL->setup(eLanguage::zeusText(44, 394), permanentButton);
        leftW->addWidget(buttonL);
    }

    if(const auto ee = dynamic_cast<eCityEventValue*>(e.get())) {
        const auto cityButtonL = new eLabeledWidget(window());

        const auto widget = new eWidget(window());
        widget->setNoPadding();
        for(int i = 0; i < 2; i++) {
            const auto cityButton = new eCityButton(window());
            const auto board = e->worldBoard();
            cityButton->initialize(board, [ee, i](const stdsptr<eWorldCity>& c) {
                const auto cid = c->cityId();
                const int iCid = static_cast<int>(cid);
                if(i) ee->setMaxCityId(iCid);
                else ee->setMinCityId(iCid);
            }, true);
            const int id = i ? ee->maxCityId() : ee->minCityId();
            const auto cid = static_cast<eCityId>(id);
            const auto cc = board->cityWithId(cid);
            if(cc) {
                cityButton->setCity(cc);
                const auto cname = cc->nameWithId();
                cityButton->setText(cname);
            } else {
                cityButton->setText(std::to_string(id));
            }
            widget->addWidget(cityButton);
        }

        widget->stackVertically(p);
        widget->fitContent();

        cityButtonL->setup(eLanguage::zeusText(44, 359), widget);
        leftW->addWidget(cityButtonL);
    }

    if(const auto ee = dynamic_cast<eAttackingCityEventValue*>(e.get())) {
        const auto cityButtonL = new eLabeledWidget(window());
        const auto cityButton = new eCityButton(window());
        const auto board = e->worldBoard();
        cityButton->initialize(board, [ee](const stdsptr<eWorldCity>& c){
            ee->setAttackingCity(c);
        });
        const auto cc = ee->attackingCity();
        cityButton->setCity(cc);
        cityButtonL->setup(eLanguage::zeusText(44, 271), cityButton);
        leftW->addWidget(cityButtonL);
    }

    if(const auto ee = dynamic_cast<eGodReasonEventValue*>(e.get())) {
        const auto godButtonL = new eLabeledWidget(window());
        const auto godButton = new eSwitchButton(window());
        godButton->setUnderline(false);
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

    if(const auto ee = dynamic_cast<eMonsterEventValue*>(e.get())) {
        const auto monsterButtonL = new eLabeledWidget(window());
        const auto act = [ee](const eMonsterType type) {
            ee->setMonster(type);
        };
        const auto monsterButton = new eMonsterButton(window());
        monsterButton->initialize(act);
        const auto iniT = ee->monster();
        monsterButton->setType(iniT);
        monsterButtonL->setup(eLanguage::zeusText(44, 175), monsterButton);
        leftW->addWidget(monsterButtonL);
    }

    const auto et = e->type();
    switch(et) {
    case eGameEventType::godAttack: {
        const auto eew = new eGodAttackEventWidget(window());
        const auto gaee = static_cast<eGodAttackEvent*>(e.get());
        eew->initialize(this, gaee);
        leftW->addWidget(eew);
    } break;
    case eGameEventType::invasion: {
        const auto eew = new eInvasionEventWidget(window());
        const auto iee = static_cast<eInvasionEvent*>(e.get());
        eew->initialize(iee);
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
    default:
        break;
    }

    const auto dateW = new eWidget(window());
    dateW->setNoPadding();

    const auto yearsButtonL = new eLabeledWidget(window());
    const auto yearsWidget = new eWidget(window());
    yearsWidget->setNoPadding();
    for(int i = 0; i < 2; i++) {
        const auto yearsButton = new eValueButton(window());
        yearsButton->setValueChangeAction([e, yearsButton, i](const int y) {
            if(i) e->setDatePlusYearsMax(y);
            else e->setDatePlusYearsMin(y);
            yearsButton->setText("+" + yearsButton->text());
        });
        yearsButton->initialize(0, 99999);
        if(i) yearsButton->setValue(e->datePlusYearsMax());
        else yearsButton->setValue(e->datePlusYearsMin());
        yearsButton->setText("+" + yearsButton->text());
        yearsWidget->addWidget(yearsButton);
    }

    yearsWidget->stackHorizontally(p);
    yearsWidget->fitContent();
    yearsButtonL->setup(eLanguage::zeusText(8, 9), yearsWidget);
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
    monthssButtonL->setup(eLanguage::zeusText(8, 5), monthsButton);
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
    daysButtonL->setup(eLanguage::zeusText(8, 45), daysButton);
    dateW->addWidget(daysButtonL);

    const auto periodButtonL = new eLabeledWidget(window());
    const auto periodW = new eWidget(window());
    periodW->setNoPadding();
    const auto periodMinButton = new eValueButton(window());
    periodMinButton->setValueChangeAction([e](const int p) {
        e->setPeriodMin(p);
    });
    periodMinButton->initialize(31, 99999);
    periodMinButton->setValue(e->periodMin());
    periodW->addWidget(periodMinButton);
    const auto periodMaxButton = new eValueButton(window());
    periodMaxButton->setValueChangeAction([e](const int p) {
        e->setPeriodMax(p);
    });
    periodMaxButton->initialize(31, 99999);
    periodMaxButton->setValue(e->periodMax());
    periodW->addWidget(periodMaxButton);
    periodW->stackHorizontally(p);
    periodW->fitContent();
    periodButtonL->setup(eLanguage::text("period:"), periodW);
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

    const auto completeButton = new eSwitchButton(window());
    completeButton->setUnderline(false);
    completeButton->setSwitchAction([e](const int v) {
        e->setEpisodeCompleteEvent(v == 3);
    });
    completeButton->addValue(eLanguage::zeusText(44, 157)); // one time
    completeButton->addValue(eLanguage::zeusText(44, 158)); // recurring
    completeButton->addValue(eLanguage::zeusText(44, 159)); // triggered
    completeButton->addValue(eLanguage::zeusText(44, 160)); // episode complete
    completeButton->fitValidContent();
    completeButton->setValue(e->episodeCompleteEvent() ? 3 : 0);
    dateW->addWidget(completeButton);

    if(!e->warnings().empty()) {
        const auto warningButtonL = new eLabeledWidget(window());
        const auto warningButton = new eValueButton(window());
        warningButton->setValueChangeAction([e](const int ms) {
            e->setWarningMonths(ms);
        });
        warningButton->initialize(0, 99999);
        warningButton->setValue(e->warningMonths());
        warningButtonL->setup(eLanguage::zeusText(44, 368), warningButton);
        dateW->addWidget(warningButtonL);
    }

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
